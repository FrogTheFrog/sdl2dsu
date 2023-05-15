// class header include
#include "clientendpoint.h"

// system includes

// local includes

//--------------------------------------------------------------------------------------------------

namespace server
{
bool operator<(const server::ClientEndpoint& lhs, const server::ClientEndpoint& rhs)
{
    return std::tie(lhs.m_client_id, lhs.m_endpoint) < std::tie(rhs.m_client_id, rhs.m_endpoint);
}
}  // namespace server
