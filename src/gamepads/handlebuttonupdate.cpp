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
    return data.m_special.m_back && data.m_abxy.m_a && data.m_abxy.m_y && data.m_dpad.m_up;
}
}  // namespace

//--------------------------------------------------------------------------------------------------

bool handleButtonUpdate(const SDL_GamepadButtonEvent& event, shared::GamepadData& data)
{
    BOOST_LOG_TRIVIAL(trace) << "button (" << static_cast<int>(event.button) << ") event "
                             << static_cast<int>(event.state) << " received for gamepad " << event.which;

    bool updated{false};
    switch (event.button)
    {
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_SOUTH:
            updated = tryModifyState(data.m_abxy.m_a, event.state);
            break;
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_EAST:
            updated = tryModifyState(data.m_abxy.m_b, event.state);
            break;
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_WEST:
            updated = tryModifyState(data.m_abxy.m_x, event.state);
            break;
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_NORTH:
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

    return updated;
}

//--------------------------------------------------------------------------------------------------

void tryToToggleSensor(const SDL_GamepadButtonEvent& event, const shared::GamepadData& data, GamepadManager& manager)
{
    switch (event.button)
    {
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_SOUTH:
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_NORTH:
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_UP:
        case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_DOWN:
            if (shouldTryToToggleSensor(data))
            {
                manager.tryChangeSensorState(event.which, std::nullopt);
            }
            break;

        default:
            break;
    }
}
}  // namespace gamepads
