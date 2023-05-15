#pragma once

// system includes
#include <boost/asio/ip/udp.hpp>
#include <functional>

// local includes

//--------------------------------------------------------------------------------------------------

namespace server
{
struct ClientEndpoint
{
    std::uint32_t                  m_client_id;
    boost::asio::ip::udp::endpoint m_endpoint;
};

//--------------------------------------------------------------------------------------------------

bool operator<(const server::ClientEndpoint& lhs, const server::ClientEndpoint& rhs);
}  // namespace server
