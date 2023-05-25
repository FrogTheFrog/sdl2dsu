#pragma once

// system includes
#include <boost/asio/ip/udp.hpp>
#include <boost/move/core.hpp>
#include <chrono>
#include <map>
#include <optional>
#include <set>

// local includes

//--------------------------------------------------------------------------------------------------

namespace server
{
class ActiveClients final
{
    BOOST_MOVABLE_BUT_NOT_COPYABLE(ActiveClients)

public:
    std::map<std::uint8_t, std::set<boost::asio::ip::udp::endpoint>>
        getRelevantEndpoints(std::set<std::uint8_t> updated_indexes);

    void updateRequestTime(const boost::asio::ip::udp::endpoint& endpoint, std::uint32_t client_id,
                           std::optional<std::uint8_t> requested_index);

private:
    void performLazyCleanup();

    struct ClientData
    {
        std::array<std::optional<std::chrono::steady_clock::time_point>, 4> m_last_request_times;
        boost::asio::ip::udp::endpoint                                      m_endpoint;
    };

    std::map<std::string, ClientData> m_clients;
};
}  // namespace server
