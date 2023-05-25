#pragma once

// system includes
#include <boost/asio/awaitable.hpp>
#include <functional>

// local includes
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
boost::asio::awaitable<void> enumerateAndWatch(shared::GamepadDataContainer& gamepad_data);
}  // namespace gamepads
