// class header include
#include "activeclients.h"

// system includes
#include <boost/log/trivial.hpp>

// local includes

//--------------------------------------------------------------------------------------------------

namespace server
{
namespace
{
using namespace std::chrono_literals;

//--------------------------------------------------------------------------------------------------

std::string makeExtendedClientId(const boost::asio::ip::udp::endpoint& endpoint, std::uint32_t client_id)
{
    return {endpoint.address().to_string() + "::" + std::to_string(endpoint.port()) + "::" + std::to_string(client_id)};
}
}  // namespace

   //--------------------------------------------------------------------------------------------------

std::map<std::uint8_t, std::set<boost::asio::ip::udp::endpoint>>
    ActiveClients::getRelevantEndpoints(std::set<std::uint8_t> updated_indexes)
{
    performLazyCleanup();

    std::map<std::uint8_t, std::set<boost::asio::ip::udp::endpoint>> relevant_endpoints;
    for (const auto updated_index : updated_indexes)
    {
        BOOST_ASSERT(updated_index < 4);
        for (const auto& client : m_clients)
        {
            if (client.second.m_last_request_times[updated_index] != std::nullopt)
            {
                relevant_endpoints[updated_index].insert(client.second.m_endpoint);
            }
        }
    }
    return relevant_endpoints;
}

//--------------------------------------------------------------------------------------------------

void ActiveClients::updateRequestTime(const boost::asio::ip::udp::endpoint& endpoint, std::uint32_t client_id,
                                      std::optional<std::uint8_t> requested_index)
{
    const auto ext_client_id{makeExtendedClientId(endpoint, client_id)};
    auto       client_data_it{m_clients.find(ext_client_id)};

    if (client_data_it == std::end(m_clients))
    {
        client_data_it = m_clients.insert({ext_client_id, ClientData{{}, endpoint}}).first;
    }

    const auto now{std::chrono::steady_clock::now()};
    if (requested_index)
    {
        BOOST_ASSERT(*requested_index < 4);
        client_data_it->second.m_last_request_times[*requested_index] = now;
    }
    else
    {
        for (auto& last_request_time : client_data_it->second.m_last_request_times)
        {
            last_request_time = now;
        }
    }
}

//--------------------------------------------------------------------------------------------------

void ActiveClients::performLazyCleanup()
{
    for (auto it = std::begin(m_clients); it != std::end(m_clients);)
    {
        auto& client_data{it->second};
        for (auto& last_request_time : client_data.m_last_request_times)
        {
            if (last_request_time)
            {
                const bool has_timed_out{std::chrono::steady_clock::now() - *last_request_time > 5s};
                if (has_timed_out)
                {
                    last_request_time = std::nullopt;
                }
            }
        }

        auto valid_request_time_it =
            std::find_if(std::begin(client_data.m_last_request_times), std::end(client_data.m_last_request_times),
                         [](const auto& request_time) { return static_cast<bool>(request_time); });

        if (valid_request_time_it != std::end(client_data.m_last_request_times))
        {
            // This that the client is still active
            ++it;
        }
        else
        {
            // Client no longer has any active requests
            it = m_clients.erase(it);
        }
    }
}
}  // namespace server
