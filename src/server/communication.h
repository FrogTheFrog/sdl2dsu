#pragma once

// system includes
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/udp.hpp>

// local includes
#include "activeclients.h"
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace server
{
std::uint32_t generateServerId();

//--------------------------------------------------------------------------------------------------

boost::asio::awaitable<void> listenAndRespond(std::uint32_t server_id, const shared::GamepadDataContainer& gamepad_data,
                                              ActiveClients& clients, boost::asio::ip::udp::socket& socket);

//--------------------------------------------------------------------------------------------------

boost::asio::awaitable<void> distributePadData(std::uint32_t                       server_id,
                                               const shared::GamepadDataContainer& gamepad_data,
                                               const std::set<std::uint8_t>& updated_indexes, ActiveClients& clients,
                                               boost::asio::ip::udp::socket& socket);
}  // namespace server
