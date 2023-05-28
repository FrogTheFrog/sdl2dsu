// system includes
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>

// local includes
#include "gamepads/enumerator.h"
#include "server/communication.h"

//--------------------------------------------------------------------------------------------------

void exceptionHandler(std::exception_ptr exception)
{
    if (exception)
    {
        std::rethrow_exception(exception);
    }
}

//--------------------------------------------------------------------------------------------------

// TODO: args
int main(/* int argc, char** argv */)
{
    try
    {
        // Prepare general coroutine stuff
        constexpr int           no_concurrency{1};
        boost::asio::io_context io_context{no_concurrency};
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) { io_context.stop(); });

        // Prepare server stuff
        const auto server_id{server::generateServerId()};
        auto       socket{boost::asio::ip::udp::socket(io_context, {boost::asio::ip::udp::v4(), 26760})};

        // Prepare coroutine containers
        server::ActiveClients        active_clients;
        shared::GamepadDataContainer gamepad_data;

        // Spawn the coroutines
        boost::asio::co_spawn(io_context, server::listenAndRespond(server_id, gamepad_data, active_clients, socket),
                              exceptionHandler);
        boost::asio::co_spawn(
            io_context,
            gamepads::enumerateAndWatch(
                [&](const auto& updated_indexes)
                { return server::distributePadData(server_id, gamepad_data, updated_indexes, active_clients, socket); },
                gamepad_data),
            exceptionHandler);

        io_context.run();
    }
    catch (const std::exception& exception)
    {
        BOOST_LOG_TRIVIAL(fatal) << exception.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
