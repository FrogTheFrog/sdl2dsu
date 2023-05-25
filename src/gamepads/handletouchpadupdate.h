#pragma once

// system includes

// local includes
#include "SDL.h"
#include "gamepadmanager.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
std::optional<std::uint8_t> handleTouchpadUpdate(const SDL_GamepadTouchpadEvent& event, GamepadManager& manager);
}  // namespace gamepads
