#pragma once

// system includes

// local includes
#include "SDL.h"
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
bool handleTouchpadUpdate(const SDL_GamepadTouchpadEvent& event, shared::GamepadData& data);
}  // namespace gamepads
