#pragma once

// system includes
#include <boost/asio/awaitable.hpp>
#include <functional>
#include <set>

// local includes
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
boost::asio::awaitable<void>
    enumerateAndWatch(std::function<boost::asio::awaitable<void>(const std::set<std::uint8_t>&)> notify_clients,
                      shared::GamepadDataContainer&                                              gamepad_data);
}  // namespace gamepads
