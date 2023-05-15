#pragma once

// system includes

// local includes
#include "SDL.h"
#include "gamepadmanager.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
std::optional<std::uint8_t> handleBatteryUpdate(const SDL_JoyBatteryEvent& event, GamepadManager& manager);
}  // namespace gamepads
