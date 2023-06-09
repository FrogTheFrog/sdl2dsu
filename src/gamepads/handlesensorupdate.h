#pragma once

// system includes

// local includes
#include "SDL.h"
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
bool handleSensorUpdate(const SDL_GamepadSensorEvent& event, shared::GamepadData& data);
}  // namespace gamepads
