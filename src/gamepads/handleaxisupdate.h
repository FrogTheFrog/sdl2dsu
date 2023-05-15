#pragma once

// system includes

// local includes
#include "SDL.h"
#include "gamepadmanager.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
std::optional<std::uint8_t> handleAxisUpdate(const SDL_GamepadAxisEvent& event, GamepadManager& manager);
}  // namespace gamepads
