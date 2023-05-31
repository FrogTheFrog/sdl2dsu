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

std::optional<SdlCleanupGuard> initializeSdl(std::string mapping_file)
{
    if (SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1") < 0
        || SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_SENSOR) < 0)
    {
        return std::nullopt;
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
            return std::nullopt;
        }

        BOOST_LOG_TRIVIAL(trace) << "Loaded mappings from: " << mapping_file;
    }

    return std::make_optional<SdlCleanupGuard>();
}
}  // namespace

//--------------------------------------------------------------------------------------------------

boost::asio::awaitable<void>
    enumerateAndWatch(std::function<boost::asio::awaitable<void>(const std::set<std::uint8_t>&)> notify_clients,
                      std::function<std::size_t()> get_number_of_active_clients,
                      const std::regex& controller_name_filter, const std::string& mapping_file,
                      bool sensor_auto_toggle, shared::GamepadDataContainer& gamepad_data)
{
    BOOST_ASSERT(notify_clients);
    BOOST_ASSERT(get_number_of_active_clients);

    const auto sdl_cleanup_guard{initializeSdl(mapping_file)};
    if (!sdl_cleanup_guard)
    {
        throw std::runtime_error(std::string{"SDL could not be initialized! SDL Error: "} + SDL_GetError());
    }

    std::set<std::uint8_t> updated_indexes;
    const auto             insert_if_updated = [&updated_indexes](const std::optional<std::uint8_t>& updated_index)
    {
        if (updated_index)
        {
            updated_indexes.insert(*updated_index);
        }
    };

    SDL_Event                             event;
    boost::asio::steady_timer             timer(co_await boost::asio::this_coro::executor);
    std::chrono::steady_clock::time_point last_sensor_check_ts{std::chrono::steady_clock::now()};
    GamepadManager                        manager{controller_name_filter, gamepad_data};
    while (true)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
                case SDL_EVENT_GAMEPAD_ADDED:
                {
                    insert_if_updated(manager.tryOpenGamepad(event.gdevice.which));
                    break;
                }
                case SDL_EVENT_GAMEPAD_REMOVED:
                {
                    insert_if_updated(manager.closeGamepad(event.gdevice.which));
                    break;
                }
                case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                {
                    insert_if_updated(handleAxisUpdate(event.gaxis, manager));
                    break;
                }
                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                case SDL_EVENT_GAMEPAD_BUTTON_UP:
                {
                    insert_if_updated(handleButtonUpdate(event.gbutton, manager));
                    break;
                }
                case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
                case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
                case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
                {
                    insert_if_updated(handleTouchpadUpdate(event.gtouchpad, manager));
                    break;
                }
                case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
                {
                    insert_if_updated(handleSensorUpdate(event.gsensor, manager));
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
                    insert_if_updated(handleBatteryUpdate(event.jbattery, manager));
                    break;
                }
                default:
                    BOOST_LOG_TRIVIAL(trace) << "Unhandled event type: " << event.type;
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
            co_await notify_clients(updated_indexes);
            updated_indexes.clear();
        }

        timer.expires_after(1ms);
        co_await timer.async_wait(boost::asio::use_awaitable);
    }
}
}  // namespace gamepads
