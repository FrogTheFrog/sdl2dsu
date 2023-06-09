// class header include
#include "handleaxisupdate.h"

// system includes
#include <boost/log/trivial.hpp>
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

bool handleAxisUpdate(const SDL_GamepadAxisEvent& event, shared::GamepadData& data)
{
    BOOST_LOG_TRIVIAL(trace) << "axis (" << static_cast<int>(event.axis) << ") value change "
                             << static_cast<int>(event.value) << " received for gamepad " << event.which;

    bool updated{false};
    switch (event.axis)
    {
        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
            updated = tryModifyTriggerState(data.m_trigger.m_left, event.value);
            break;
        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
            updated = tryModifyTriggerState(data.m_trigger.m_right, event.value);
            break;

        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_LEFTX:
            updated = tryModifyAxisState(data.m_left_stick.m_x, event.value);
            break;
        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_LEFTY:
            updated = tryModifyAxisState(data.m_left_stick.m_y, event.value);
            break;

        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_RIGHTX:
            updated = tryModifyAxisState(data.m_right_stick.m_x, event.value);
            break;
        case SDL_GamepadAxis::SDL_GAMEPAD_AXIS_RIGHTY:
            updated = tryModifyAxisState(data.m_right_stick.m_y, event.value);
            break;

        default:
            break;
    }

    return updated;
}
}  // namespace gamepads
