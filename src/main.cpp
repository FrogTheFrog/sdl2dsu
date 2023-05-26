// system includes
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>

// local includes
#include "gamepads/enumerator.h"
#include "server/communication.h"

//--------------------------------------------------------------------------------------------------

boost::asio::awaitable<void> mainCoRoutine()
{
    using namespace boost::asio::experimental::awaitable_operators;

    const auto server_id{server::generateServerId()};
    auto       executor{co_await boost::asio::this_coro::executor};
    auto       socket{boost::asio::ip::udp::socket(executor, {boost::asio::ip::udp::v4(), 26760})};

    server::ActiveClients        active_clients;
    shared::GamepadDataContainer gamepad_data;

    co_await (
        server::listenAndRespond(server_id, gamepad_data, active_clients, socket)
        && gamepads::enumerateAndWatch(
            [&](const auto& updated_indexes)
            { return server::distributePadData(server_id, gamepad_data, updated_indexes, active_clients, socket); },
            gamepad_data));
}

//--------------------------------------------------------------------------------------------------

int main(/* int argc, char** argv */)
{
    try
    {
        constexpr int           no_concurrency{1};
        boost::asio::io_context io_context{no_concurrency};

        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) { io_context.stop(); });

        boost::asio::co_spawn(io_context, mainCoRoutine(), boost::asio::detached);
        io_context.run();
    }
    catch (std::exception& exception)
    {
        BOOST_LOG_TRIVIAL(error) << "Exception: " << exception.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
