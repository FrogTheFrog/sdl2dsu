// class header include
#include "clientendpointcounter.h"

// system includes

// local includes

//--------------------------------------------------------------------------------------------------

namespace server
{
bool operator<(const server::ClientEndpointCounter& lhs, const server::ClientEndpointCounter& rhs)
{
    return std::tie(lhs.m_client_endpoint, lhs.m_packet_counter)
           < std::tie(rhs.m_client_endpoint, rhs.m_packet_counter);
}
}  // namespace server
