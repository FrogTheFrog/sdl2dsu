// class header include
#include "communication.h"

// system includes
#include <boost/algorithm/string/join.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/log/trivial.hpp>
#include <random>

// local includes
#include "deserialiser.h"
#include "serialiser.h"

//--------------------------------------------------------------------------------------------------

namespace server
{
std::uint32_t generateServerId()
{
    std::random_device                           seed;
    std::default_random_engine                   engine(seed());
    std::uniform_int_distribution<std::uint32_t> uniform_dist(1, std::numeric_limits<std::uint32_t>::max());
    return uniform_dist(engine);
}

//--------------------------------------------------------------------------------------------------

boost::asio::awaitable<void> listenAndRespond(std::uint32_t server_id, const shared::GamepadDataContainer& gamepad_data,
                                              ActiveClients& clients, boost::asio::ip::udp::socket& socket)
{
    BOOST_LOG_TRIVIAL(info) << "Server listening on " << socket.local_endpoint();

    std::array<std::uint8_t, 1024> data;
    for (;;)
    {
        boost::asio::ip::udp::endpoint client;
        const std::size_t              data_size{
            co_await socket.async_receive_from(boost::asio::buffer(data), client, boost::asio::use_awaitable)};

        const auto result{deserialise({std::begin(data), std::begin(data) + data_size})};
        if (!result)
        {
            continue;
        }

        std::vector<std::vector<std::uint8_t>> responses;
        if (std::get_if<VersionRequest>(&*result))
        {
            responses = {serialise(VersionResponse{}, server_id)};
        }
        else if (const auto ports_request = std::get_if<ListPortsRequest>(&*result))
        {
            responses = serialise(ListPortsResponse{ports_request->m_requested_indexes, gamepad_data}, server_id);
        }
        else if (const auto data_request = std::get_if<PadDataRequest>(&*result))
        {
            clients.updateRequestTime(client, data_request->m_client_id, data_request->m_requested_indexes);
        }

        for (const auto& response : responses)
        {
            BOOST_ASSERT(!response.empty());
            co_await socket.async_send_to(boost::asio::buffer(response), client, boost::asio::use_awaitable);
        }
    }
}

//--------------------------------------------------------------------------------------------------

boost::asio::awaitable<void> distributePadData(std::uint32_t                       server_id,
                                               const shared::GamepadDataContainer& gamepad_data,
                                               const std::uint8_t index, ActiveClients& clients,
                                               boost::asio::ip::udp::socket& socket)
{
    BOOST_ASSERT(index < 4);
    BOOST_LOG_TRIVIAL(debug) << "Sending updates for pad index: " << static_cast<int>(index);

    std::map<boost::asio::ip::udp::endpoint, std::vector<std::vector<std::uint8_t>>> data_to_send;
    const auto& relevant_endpoints{clients.getRelevantEndpoints(index)};
    for (const auto& relevant_endpoint : relevant_endpoints)
    {
        BOOST_LOG_TRIVIAL(debug) << "Serializing response for " << relevant_endpoint.m_client_endpoint.m_endpoint
                                 << ", for pad index " << static_cast<int>(index);

        auto response{serialise(PadDataResponse{index, relevant_endpoint.m_client_endpoint.m_client_id,
                                                relevant_endpoint.m_packet_counter, gamepad_data[index]},
                                server_id)};

        BOOST_ASSERT(!response.empty());
        data_to_send[relevant_endpoint.m_client_endpoint.m_endpoint].push_back(std::move(response));
    }

    for (const auto& item : data_to_send)
    {
        const auto  endpoint{item.first};
        const auto& data_list{item.second};

        for (const auto& data : data_list)
        {
            co_await socket.async_send_to(boost::asio::buffer(data), endpoint, boost::asio::use_awaitable);
        }
    }
}
}  // namespace server
