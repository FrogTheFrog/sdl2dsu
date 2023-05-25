// class header include
#include "handlebuttonupdate.h"

// system includes

// local includes

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
namespace
{
bool tryModifyState(bool& from, std::uint8_t to)
{
    const bool is_pressed{to == SDL_PRESSED};
    if (from == is_pressed)
    {
        return false;
    }

    from = is_pressed;
    return true;
}
}  // namespace

//--------------------------------------------------------------------------------------------------

std::optional<std::uint8_t> handleButtonUpdate(const SDL_GamepadButtonEvent& event, GamepadManager& manager)
{
    switch (event.button)
    {
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_A:
            return manager.tryUpdateData(event.which,
                                         [&event](auto& data) { return tryModifyState(data.m_abxy.m_a, event.state); });
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_B:
            return manager.tryUpdateData(event.which,
                                         [&event](auto& data) { return tryModifyState(data.m_abxy.m_b, event.state); });
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_X:
            return manager.tryUpdateData(event.which,
                                         [&event](auto& data) { return tryModifyState(data.m_abxy.m_x, event.state); });
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_Y:
            return manager.tryUpdateData(event.which,
                                         [&event](auto& data) { return tryModifyState(data.m_abxy.m_y, event.state); });

        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_UP:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_dpad.m_up, event.state); });
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_DOWN:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_dpad.m_down, event.state); });
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_LEFT:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_dpad.m_left, event.state); });
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_dpad.m_right, event.state); });

        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_BACK:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_special.m_back, event.state); });
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_GUIDE:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_special.m_guide, event.state); });
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_START:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_special.m_start, event.state); });

        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_shoulder.m_left, event.state); });
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_shoulder.m_right, event.state); });

        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_LEFT_STICK:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_left_stick.m_pressed, event.state); });
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_RIGHT_STICK:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_right_stick.m_pressed, event.state); });

        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_TOUCHPAD:
            return manager.tryUpdateData(event.which, [&event](auto& data)
                                         { return tryModifyState(data.m_touchpad.m_pressed, event.state); });

        default:
            BOOST_LOG_TRIVIAL(trace) << "button (" << event.button << ") event " << event.state
                                     << " received for gamepad " << event.which;
            return std::nullopt;
    }
}
}  // namespace gamepads
