#pragma once

// system includes
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/udp.hpp>

// local includes
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace server
{
std::uint32_t generateServerId();

//--------------------------------------------------------------------------------------------------

boost::asio::awaitable<void> listenAndRespond(std::uint32_t server_id, const shared::GamepadDataContainer& gamepad_data,
                                              boost::asio::ip::udp::socket& socket);
}  // namespace server
