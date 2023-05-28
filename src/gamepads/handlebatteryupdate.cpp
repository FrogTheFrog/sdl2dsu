// class header include
#include "handleaxisupdate.h"

// system includes

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

std::optional<std::uint8_t> handleBatteryUpdate(const SDL_JoyBatteryEvent& event, GamepadManager& manager)
{
    BOOST_LOG_TRIVIAL(trace) << "battery level value change " << event.level << " received for gamepad " << event.which;

    switch (event.level)
    {
        case SDL_JoystickPowerLevel::SDL_JOYSTICK_POWER_EMPTY:
            return manager.tryUpdateData(event.which,
                                         [](auto& data) { return tryModifyState(data.m_battery, BL::Empty); });
        case SDL_JoystickPowerLevel::SDL_JOYSTICK_POWER_LOW:
            return manager.tryUpdateData(event.which,
                                         [](auto& data) { return tryModifyState(data.m_battery, BL::Low); });
        case SDL_JoystickPowerLevel::SDL_JOYSTICK_POWER_MEDIUM:
            return manager.tryUpdateData(event.which,
                                         [](auto& data) { return tryModifyState(data.m_battery, BL::Medium); });
        case SDL_JoystickPowerLevel::SDL_JOYSTICK_POWER_FULL:
            return manager.tryUpdateData(event.which,
                                         [](auto& data) { return tryModifyState(data.m_battery, BL::Full); });
        case SDL_JoystickPowerLevel::SDL_JOYSTICK_POWER_WIRED:
            return manager.tryUpdateData(event.which,
                                         [](auto& data) { return tryModifyState(data.m_battery, BL::Wired); });

        default:
            return std::nullopt;
    }
}
}  // namespace gamepads
