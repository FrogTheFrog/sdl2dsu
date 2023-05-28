// class header include
#include "handleaxisupdate.h"

// system includes
#include <limits>

// local includes

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
namespace
{
std::uint8_t remapRange(std::int16_t value, const std::int32_t old_min, const std::int32_t new_min,
                        const std::int32_t old_range, const std::int32_t new_range)
{
    return static_cast<std::uint8_t>((((static_cast<std::int32_t>(value) - old_min) * new_range) / old_range)
                                     + new_min);
}

//--------------------------------------------------------------------------------------------------

bool tryModifyTriggerState(std::uint8_t& from, std::int16_t to)
{
    constexpr std::int32_t old_min{0};
    constexpr std::int32_t new_min{std::numeric_limits<std::uint8_t>::min()};
    constexpr std::int32_t old_range{SDL_JOYSTICK_AXIS_MAX - old_min};
    constexpr std::int32_t new_range{std::numeric_limits<std::uint8_t>::max() - new_min};

    const auto converted_to{remapRange(to, old_min, new_min, old_range, new_range)};
    if (from == converted_to)
    {
        return false;
    }

    from = converted_to;
    return true;
}

//--------------------------------------------------------------------------------------------------

bool tryModifyAxisState(std::uint8_t& from, std::int16_t to)
{
    constexpr std::int32_t old_min{SDL_JOYSTICK_AXIS_MIN};
    constexpr std::int32_t new_min{std::numeric_limits<std::uint8_t>::min()};
    constexpr std::int32_t old_range{SDL_JOYSTICK_AXIS_MAX - old_min};
    constexpr std::int32_t new_range{std::numeric_limits<std::uint8_t>::max() - new_min};

    const auto converted_to{remapRange(to, old_min, new_min, old_range, new_range)};
    if (from == converted_to)
    {
        return false;
    }

    from = converted_to;
    return true;
}
}  // namespace

//--------------------------------------------------------------------------------------------------

std::optional<std::uint8_t> handleAxisUpdate(const SDL_GamepadAxisEvent& event, GamepadManager& manager)
{
    BOOST_LOG_TRIVIAL(trace) << "axis (" << event.axis << ") value change " << event.value << " received for gamepad "
                             << event.which;

    switch (event.axis)
    {
        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyTriggerState(data.m_trigger.m_left, event.value); });
        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyTriggerState(data.m_trigger.m_right, event.value); });

        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_LEFTX:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyAxisState(data.m_left_stick.m_x, event.value); });
        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_LEFTY:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyAxisState(data.m_left_stick.m_y, event.value); });

        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_RIGHTX:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyAxisState(data.m_right_stick.m_x, event.value); });
        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_RIGHTY:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyAxisState(data.m_right_stick.m_y, event.value); });

        default:
            return std::nullopt;
    }
}
}  // namespace gamepads
