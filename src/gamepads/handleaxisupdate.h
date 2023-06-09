#pragma once

// system includes

// local includes
#include "SDL.h"
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
bool handleAxisUpdate(const SDL_GamepadAxisEvent& event, shared::GamepadData& data);
}  // namespace gamepads
