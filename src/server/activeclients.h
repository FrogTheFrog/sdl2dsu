#pragma once

// system includes
#include <boost/move/core.hpp>
#include <chrono>
#include <map>
#include <optional>
#include <set>

// local includes
#include "clientendpointcounter.h"

//--------------------------------------------------------------------------------------------------

namespace server
{
class ActiveClients final
{
    BOOST_MOVABLE_BUT_NOT_COPYABLE(ActiveClients)

public:
    explicit ActiveClients() = default;

    std::map<std::uint8_t, std::set<ClientEndpointCounter>>
                getRelevantEndpoints(std::set<std::uint8_t> updated_indexes);
    void        updateRequestTime(const boost::asio::ip::udp::endpoint& endpoint, std::uint32_t client_id,
                                  std::set<std::uint8_t> requested_indexes);
    std::size_t getNumberOfClients() const;

private:
    void performLazyCleanup() const;

    struct ClientData
    {
        std::chrono::steady_clock::time_point m_last_request_time;
        std::uint32_t                         m_packet_counter{0};
    };

    using ClientDataPerPad = std::array<std::optional<ClientData>, 4>;
    mutable std::map<ClientEndpoint, ClientDataPerPad> m_clients;
};
}  // namespace server
