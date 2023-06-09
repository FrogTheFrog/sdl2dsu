#pragma once

// system includes

// local includes
#include "SDL.h"
#include "gamepadmanager.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
bool handleButtonUpdate(const SDL_GamepadButtonEvent& event, shared::GamepadData& data);

//--------------------------------------------------------------------------------------------------

void tryToToggleSensor(const SDL_GamepadButtonEvent& event, const shared::GamepadData& data, GamepadManager& manager);
}  // namespace gamepads
