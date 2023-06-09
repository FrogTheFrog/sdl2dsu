#pragma once

// system includes

// local includes
#include "SDL.h"
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
bool handleBatteryUpdate(const SDL_JoyBatteryEvent& event, shared::GamepadData& data);
}  // namespace gamepads
