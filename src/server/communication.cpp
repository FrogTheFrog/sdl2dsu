// class header include
#include "communication.h"

// system includes
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
                                              boost::asio::ip::udp::socket& socket)
{
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
        else if (const auto request = std::get_if<ListPortsRequest>(&*result))
        {
            responses = serialise(ListPortsResponse{request->m_requested_indexes, gamepad_data}, server_id);
        }
        else if (const auto request = std::get_if<PadDataRequest>(&*result))
        {
            // TODO
        }

        for (const auto& response : responses)
        {
            if (!response.empty())
            {
                co_await socket.async_send_to(boost::asio::buffer(response), client, boost::asio::use_awaitable);
            }
        }
    }
}
}  // namespace server
