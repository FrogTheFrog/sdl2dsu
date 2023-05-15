#pragma once

// system includes

// local includes
#include "SDL.h"
#include "gamepadmanager.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
std::optional<std::uint8_t> handleSensorUpdate(const SDL_GamepadSensorEvent& event, GamepadManager& manager);
}  // namespace gamepads
