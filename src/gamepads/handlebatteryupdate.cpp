// class header include
#include "handleaxisupdate.h"

// system includes
#include <boost/log/trivial.hpp>

// local includes

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
namespace
{
using BL = shared::details::BatteryLevel;

//--------------------------------------------------------------------------------------------------

bool tryModifyState(BL& from, BL to)
{
    if (from == to)
    {
        return false;
    }

    from = to;
    return true;
}
}  // namespace

//--------------------------------------------------------------------------------------------------

bool handleBatteryUpdate(const SDL_JoyBatteryEvent& event, shared::GamepadData& data)
{
    BOOST_LOG_TRIVIAL(trace) << "battery level value change " << event.level << " received for gamepad " << event.which;

    bool updated{false};
    switch (event.level)
    {
        case SDL_JoystickPowerLevel::SDL_JOYSTICK_POWER_EMPTY:
            updated = tryModifyState(data.m_battery, BL::Empty);
            break;
        case SDL_JoystickPowerLevel::SDL_JOYSTICK_POWER_LOW:
            updated = tryModifyState(data.m_battery, BL::Low);
            break;
        case SDL_JoystickPowerLevel::SDL_JOYSTICK_POWER_MEDIUM:
            updated = tryModifyState(data.m_battery, BL::Medium);
            break;
        case SDL_JoystickPowerLevel::SDL_JOYSTICK_POWER_FULL:
            updated = tryModifyState(data.m_battery, BL::Full);
            break;
        case SDL_JoystickPowerLevel::SDL_JOYSTICK_POWER_WIRED:
            updated = tryModifyState(data.m_battery, BL::Wired);
            break;

        default:
            break;
    }

    return updated;
}
}  // namespace gamepads
