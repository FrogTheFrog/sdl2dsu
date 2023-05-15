#pragma once

// system includes

// local includes
#include "SDL.h"
#include "gamepadmanager.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
std::optional<std::uint8_t> handleButtonUpdate(const SDL_GamepadButtonEvent& event, GamepadManager& manager);
}  // namespace gamepads
