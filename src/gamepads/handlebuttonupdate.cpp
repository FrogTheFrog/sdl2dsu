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

//--------------------------------------------------------------------------------------------------

bool shouldTryToToggleSensor(const shared::GamepadData& data)
{
    return data.m_abxy.m_a && data.m_abxy.m_y && data.m_dpad.m_up && data.m_dpad.m_down;
}
}  // namespace

//--------------------------------------------------------------------------------------------------

std::optional<std::uint8_t> handleButtonUpdate(const SDL_GamepadButtonEvent& event, GamepadManager& manager)
{
    BOOST_LOG_TRIVIAL(trace) << "button (" << event.button << ") event " << event.state << " received for gamepad "
                             << event.which;

    return manager.tryUpdateData(event.which,
                                 [&event, &manager](shared::GamepadData& data)
                                 {
                                     bool updated{false};
                                     switch (event.button)
                                     {
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_A:
                                             updated = tryModifyState(data.m_abxy.m_a, event.state);
                                             break;
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_B:
                                             updated = tryModifyState(data.m_abxy.m_b, event.state);
                                             break;
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_X:
                                             updated = tryModifyState(data.m_abxy.m_x, event.state);
                                             break;
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_Y:
                                             updated = tryModifyState(data.m_abxy.m_y, event.state);
                                             break;

                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_UP:
                                             updated = tryModifyState(data.m_dpad.m_up, event.state);
                                             break;
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                                             updated = tryModifyState(data.m_dpad.m_down, event.state);
                                             break;
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                                             updated = tryModifyState(data.m_dpad.m_left, event.state);
                                             break;
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                                             updated = tryModifyState(data.m_dpad.m_right, event.state);
                                             break;

                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_BACK:
                                             updated = tryModifyState(data.m_special.m_back, event.state);
                                             break;
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_GUIDE:
                                             updated = tryModifyState(data.m_special.m_guide, event.state);
                                             break;
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_START:
                                             updated = tryModifyState(data.m_special.m_start, event.state);
                                             break;

                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
                                             updated = tryModifyState(data.m_shoulder.m_left, event.state);
                                             break;
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
                                             updated = tryModifyState(data.m_shoulder.m_right, event.state);
                                             break;

                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_LEFT_STICK:
                                             updated = tryModifyState(data.m_left_stick.m_pressed, event.state);
                                             break;
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_RIGHT_STICK:
                                             updated = tryModifyState(data.m_right_stick.m_pressed, event.state);
                                             break;

                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_TOUCHPAD:
                                             updated = tryModifyState(data.m_touchpad.m_pressed, event.state);
                                             break;

                                         default:
                                             break;
                                     }

                                     // Handle the sensor toggle
                                     switch (event.button)
                                     {
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_A:
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_Y:
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_UP:
                                         case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                                             if (updated && shouldTryToToggleSensor(data))
                                             {
                                                 manager.tryChangeSensorState(event.which, std::nullopt);
                                             }
                                             break;

                                         default:
                                             break;
                                     }

                                     return updated;
                                 });
}
}  // namespace gamepads
