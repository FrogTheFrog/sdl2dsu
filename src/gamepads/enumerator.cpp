// class header include
#include "enumerator.h"

// system includes
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <filesystem>
#include <optional>
#include <stdexcept>

// local includes
#include "gamepadmanager.h"
#include "handleaxisupdate.h"
#include "handlebatteryupdate.h"
#include "handlebuttonupdate.h"
#include "handlesensorupdate.h"
#include "handletouchpadupdate.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
namespace
{
using namespace std::chrono_literals;

//--------------------------------------------------------------------------------------------------

class SdlCleanupGuard final
{
    BOOST_MOVABLE_BUT_NOT_COPYABLE(SdlCleanupGuard)

public:
    explicit SdlCleanupGuard() = default;
    ~SdlCleanupGuard()
    {
        SDL_Quit();
    }
};

//--------------------------------------------------------------------------------------------------

SdlCleanupGuard initializeSdl(std::string mapping_file)
{
    if (SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1") < 0
        || SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_SENSOR) < 0)
    {
        throw std::runtime_error(std::string{"SDL could not be initialized! SDL Error: "} + SDL_GetError());
    }

    if (mapping_file.empty())
    {
        if (std::filesystem::exists("gamecontrollerdb.txt"))
        {
            mapping_file = "gamecontrollerdb.txt";
        }
    }

    if (!mapping_file.empty())
    {
        if (!std::filesystem::exists(mapping_file))
        {
            throw std::runtime_error(std::string{"Mapping file does not exist: "} + mapping_file);
        }

        if (SDL_AddGamepadMappingsFromFile(mapping_file.c_str()) < 0)
        {
            throw std::runtime_error(std::string{"SDL could not parse mapping file! SDL Error: "} + SDL_GetError());
        }

        BOOST_LOG_TRIVIAL(info) << "Loaded mappings from: " << mapping_file;
    }

    return SdlCleanupGuard();
}
}  // namespace

//--------------------------------------------------------------------------------------------------

boost::asio::awaitable<void>
    enumerateAndWatch(std::function<boost::asio::awaitable<void>(const std::uint8_t)> notify_clients,
                      std::function<std::size_t()>                                    get_number_of_active_clients,
                      const std::regex& controller_name_filter, const std::string& mapping_file,
                      bool sensor_auto_toggle, shared::GamepadDataContainer& gamepad_data)
{
    BOOST_ASSERT(notify_clients);
    BOOST_ASSERT(get_number_of_active_clients);

    const auto                            sdl_cleanup_guard{initializeSdl(mapping_file)};
    boost::asio::steady_timer             timer(co_await boost::asio::this_coro::executor);
    std::chrono::steady_clock::time_point last_sensor_check_ts{std::chrono::steady_clock::now()};
    GamepadManager                        manager{controller_name_filter, gamepad_data};

    std::set<std::uint8_t> updated_indexes;
    shared::GamepadData*   last_device_data{nullptr};
    std::uint32_t          last_device_id{0};
    const auto             unload_device_data = [&last_device_data, &last_device_id]()
    {
        last_device_data = nullptr;
        last_device_id   = 0;
    };
    const auto load_device_data = [&last_device_data, &last_device_id, &manager](const auto& event) -> bool
    {
        const std::uint32_t device_id{event.which};
        if (device_id != last_device_id || last_device_data == nullptr)
        {
            last_device_data = manager.tryGetData(device_id);
            if (!last_device_data)
            {
                BOOST_LOG_TRIVIAL(error) << "gamepad with id " << device_id << " has no data!";
                return false;
            }
        }
        return true;
    };
    const auto data_needs_to_be_sent_now = [&last_device_data, &updated_indexes](const auto& event) -> bool
    {
        BOOST_ASSERT(last_device_data);
        if (last_device_data->m_pad_info.m_update_ts != event.timestamp
            && updated_indexes.contains(last_device_data->m_pad_info.m_index))
        {
            updated_indexes.erase(last_device_data->m_pad_info.m_index);
            return true;
        }
        return false;
    };
    const auto try_update_data = [&last_device_data, &updated_indexes](const auto& event, auto&& modifier)
    {
        BOOST_ASSERT(last_device_data);
        const bool result{modifier(event, *last_device_data)};
        if (result)
        {
            last_device_data->m_pad_info.m_update_ts = event.timestamp;
            updated_indexes.insert(last_device_data->m_pad_info.m_index);
        }
        return result;
    };

    SDL_Event base_event;
    while (true)
    {
        while (SDL_PollEvent(&base_event) != 0)
        {
            switch (base_event.type)
            {
                case SDL_EVENT_GAMEPAD_ADDED:
                {
                    const auto new_index{manager.tryOpenGamepad(base_event.gdevice.which)};
                    if (new_index)
                    {
                        updated_indexes.erase(*new_index);
                        co_await notify_clients(*new_index);
                    }
                    break;
                }
                case SDL_EVENT_GAMEPAD_REMOVED:
                {
                    unload_device_data();
                    const auto pending_index{manager.closeGamepad(base_event.gdevice.which)};
                    if (pending_index)
                    {
                        updated_indexes.erase(*pending_index);
                        co_await notify_clients(*pending_index);
                    }
                    break;
                }
                case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                {
                    const auto& event{base_event.gaxis};
                    if (load_device_data(event))
                    {
                        if (data_needs_to_be_sent_now(event))
                        {
                            co_await notify_clients(last_device_data->m_pad_info.m_index);
                        }

                        try_update_data(event, handleAxisUpdate);
                    }
                    break;
                }
                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                case SDL_EVENT_GAMEPAD_BUTTON_UP:
                {
                    const auto& event{base_event.gbutton};
                    if (load_device_data(event))
                    {
                        if (data_needs_to_be_sent_now(event))
                        {
                            co_await notify_clients(last_device_data->m_pad_info.m_index);
                        }

                        if (try_update_data(event, handleButtonUpdate))
                        {
                            BOOST_ASSERT(last_device_data);
                            tryToToggleSensor(event, *last_device_data, manager);
                        }
                    }
                    break;
                }
                case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
                case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
                case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
                {
                    const auto& event{base_event.gtouchpad};
                    if (load_device_data(event))
                    {
                        if (data_needs_to_be_sent_now(event))
                        {
                            co_await notify_clients(last_device_data->m_pad_info.m_index);
                        }

                        try_update_data(event, handleTouchpadUpdate);
                    }
                    break;
                }
                case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
                {
                    const auto& event{base_event.gsensor};
                    if (load_device_data(event))
                    {
                        if (data_needs_to_be_sent_now(event))
                        {
                            co_await notify_clients(last_device_data->m_pad_info.m_index);
                        }

                        try_update_data(event, handleSensorUpdate);
                    }
                    break;
                }
                case SDL_EVENT_JOYSTICK_AXIS_MOTION:
                case SDL_EVENT_JOYSTICK_HAT_MOTION:
                case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
                case SDL_EVENT_JOYSTICK_BUTTON_UP:
                case SDL_EVENT_JOYSTICK_ADDED:
                case SDL_EVENT_JOYSTICK_REMOVED:
                {
                    // Silently ignore these redundant events
                    break;
                }
                case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
                {
                    const auto& event{base_event.jbattery};
                    if (load_device_data(event))
                    {
                        if (data_needs_to_be_sent_now(event))
                        {
                            co_await notify_clients(last_device_data->m_pad_info.m_index);
                        }

                        try_update_data(event, handleBatteryUpdate);
                    }
                    break;
                }
                default:
                    BOOST_LOG_TRIVIAL(trace) << "Unhandled event type: " << base_event.type;
                    break;
            }
        }

        const auto now{std::chrono::steady_clock::now()};
        if (sensor_auto_toggle && (now - last_sensor_check_ts) > 10s)
        {
            const bool enable{get_number_of_active_clients() > 0};
            BOOST_LOG_TRIVIAL(debug) << "Changing sensor state for open controllers to "
                                     << (enable ? "ENABLED" : "DISABLED");

            last_sensor_check_ts = now;
            manager.tryChangeSensorStateForAll(enable);
        }

        if (!updated_indexes.empty())
        {
            for (const auto index : updated_indexes)
            {
                co_await notify_clients(index);
            }
            updated_indexes.clear();
        }
        else
        {
            timer.expires_after(1ms);
            co_await timer.async_wait(boost::asio::use_awaitable);
        }
    }
}
}  // namespace gamepads
