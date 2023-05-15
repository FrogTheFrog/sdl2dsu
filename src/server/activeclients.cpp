// class header include
#include "activeclients.h"

// system includes
#include <boost/log/trivial.hpp>

// local includes

//--------------------------------------------------------------------------------------------------

namespace server
{
using namespace std::chrono_literals;

//--------------------------------------------------------------------------------------------------

std::map<std::uint8_t, std::set<ClientEndpointCounter>>
    ActiveClients::getRelevantEndpoints(std::set<std::uint8_t> updated_indexes)
{
    performLazyCleanup();

    std::map<std::uint8_t, std::set<ClientEndpointCounter>> relevant_endpoints;
    for (const auto updated_index : updated_indexes)
    {
        BOOST_ASSERT(updated_index < 4);
        for (auto& client : m_clients)
        {
            auto& client_data{client.second[updated_index]};
            if (client_data != std::nullopt)
            {
                // Note: incrementing counter here
                relevant_endpoints[updated_index].insert({client.first, client_data->m_packet_counter++});
            }
        }
    }
    return relevant_endpoints;
}

//--------------------------------------------------------------------------------------------------

void ActiveClients::updateRequestTime(const boost::asio::ip::udp::endpoint& endpoint, std::uint32_t client_id,
                                      std::set<std::uint8_t> requested_indexes)
{
    const ClientEndpoint client_endpoint{client_id, endpoint};
    auto                 pad_data_it{m_clients.find(client_endpoint)};

    if (pad_data_it == std::end(m_clients))
    {
        pad_data_it = m_clients.try_emplace(client_endpoint, ClientDataPerPad{}).first;
    }

    const auto now{std::chrono::steady_clock::now()};
    const auto set_client_data_timestamp = [&now, &endpoint, client_id](std::optional<ClientData>& client_data)
    {
        if (client_data)
        {
            client_data->m_last_request_time = now;
        }
        else
        {
            client_data = {now, 0};
        }
    };

    if (!requested_indexes.empty())
    {
        for (const auto index : requested_indexes)
        {
            BOOST_ASSERT(index < 4);

            BOOST_LOG_TRIVIAL(debug) << "Client " << client_id << " (" << endpoint << ") updated timestamp for pad "
                                     << static_cast<int>(index);
            set_client_data_timestamp(pad_data_it->second[index]);
        }
    }
    else
    {
        BOOST_LOG_TRIVIAL(debug) << "Client " << client_id << " (" << endpoint << ") updated timestamp for all pads.";
        for (auto& client_data : pad_data_it->second)
        {
            set_client_data_timestamp(client_data);
        }
    }
}

//--------------------------------------------------------------------------------------------------

std::size_t ActiveClients::getNumberOfClients() const
{
    performLazyCleanup();

    return m_clients.size();
}

//--------------------------------------------------------------------------------------------------

void ActiveClients::performLazyCleanup() const
{
    for (auto it = std::begin(m_clients); it != std::end(m_clients);)
    {
        auto& pad_data{it->second};
        for (std::size_t i = 0; i < pad_data.size(); ++i)
        {
            auto& client_data{pad_data[i]};
            if (client_data)
            {
                const bool has_timed_out{std::chrono::steady_clock::now() - client_data->m_last_request_time > 5s};
                if (has_timed_out)
                {
                    BOOST_LOG_TRIVIAL(debug) << "Client " << it->first.m_client_id << " (" << it->first.m_endpoint
                                             << ") has timed out for pad " << static_cast<int>(i);
                    client_data = std::nullopt;
                }
            }
        }

        auto valid_client_data_it =
            std::find_if(std::begin(pad_data), std::end(pad_data),
                         [](const auto& client_data) { return static_cast<bool>(client_data); });

        if (valid_client_data_it != std::end(pad_data))
        {
            // This that the client is still active
            ++it;
        }
        else
        {
            BOOST_LOG_TRIVIAL(debug) << "Client " << it->first.m_client_id << " (" << it->first.m_endpoint
                                     << ") is no longer connected ";

            // Client no longer has any active requests
            it = m_clients.erase(it);
        }
    }
}
}  // namespace server
