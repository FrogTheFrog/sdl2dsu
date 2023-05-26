#pragma once

// system includes

// local includes
#include "clientendpoint.h"

//--------------------------------------------------------------------------------------------------

namespace server
{
struct ClientEndpointCounter
{
    ClientEndpoint m_client_endpoint;
    std::uint32_t  m_packet_counter;
};

//--------------------------------------------------------------------------------------------------

bool operator<(const server::ClientEndpointCounter& lhs, const server::ClientEndpointCounter& rhs);
}  // namespace server
