// system includes
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <iostream>

// local includes
#include "gamepads/enumerator.h"
#include "server/communication.h"

//--------------------------------------------------------------------------------------------------

namespace
{
void exceptionHandler(std::exception_ptr exception)
{
    if (exception)
    {
        std::rethrow_exception(exception);
    }
}

//--------------------------------------------------------------------------------------------------

bool parseProgramArgs(int argc, const char* const* const argv, std::uint16_t& port, std::regex& controller_name_filter,
                      std::string& mapping_file, bool& sensor_auto_toggle)
{
    try
    {
        namespace po = boost::program_options;
        using sl     = boost::log::trivial::severity_level;

        sl                      log_severity;
        std::string             filter;
        bool                    no_auto_toggle;
        po::options_description desc("Available options");
        desc.add_options()                                                                                            //
            ("help", "print this help message")                                                                       //
            ("port", po::value<std::uint16_t>(&port)->required(), "port to use for DSU server")                       //
            ("filter", po::value<std::string>(&filter)->default_value(".*"),                                          //
             "regular expression (case-insensitive) to filter the controller names that we want to observe")          //
            ("noautotoggle", po::value<bool>(&no_auto_toggle)->implicit_value(true),                                  //
             "disable the lazy automatic sensor toggle depending on the client activity")                             //
            ("mappingfile", po::value<std::string>(&mapping_file),                                                    //
             "path to the optional mapping file to be used. Will try to load gamecontrollerdb.txt by default if it "  //
             "exists in the same directory.")                                                                         //
            ("loglevel", po::value<sl>(&log_severity)->default_value(sl::info),                                       //
             "log level to output (trace, debug, info, warning, error, fatal)");

        po::variables_map vars;
        po::store(po::parse_command_line(argc, argv, desc), vars);

        if (vars.contains("help"))
        {
            std::cout << std::endl
                      << "Usage example:" << std::endl
                      << "  sdl2dsu --port 26760 --filter \"Dualsense\"" << std::endl
                      << std::endl;
            std::cout << "Sensor activation:" << std::endl
                      << "  By default, sensors will be automatically turned ON/OFF depending on whether there is a "
                         "client or not."
                      << std::endl
                      << "  They can also be toggled ON/OFF by pressing A+Y+DPAD_UP+BACK (or their equivalent) at the "
                         "same time."
                      << std::endl
                      << std::endl;
            std::cout << desc << std::endl;
            return false;
        }

        po::notify(vars);
        controller_name_filter = std::regex{filter, std::regex_constants::icase | std::regex_constants::ECMAScript};
        sensor_auto_toggle     = !no_auto_toggle;
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= log_severity);
    }
    catch (const std::exception& exception)
    {
        std::cout << exception.what() << std::endl;
        return false;
    }

    return true;
}
}  // namespace

//--------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    try
    {
        std::uint16_t port;
        std::regex    controller_name_filter;
        std::string   mapping_file;
        bool          sensor_auto_toggle;
        if (!parseProgramArgs(argc, argv, port, controller_name_filter, mapping_file, sensor_auto_toggle))
        {
            return EXIT_FAILURE;
        }

        // Prepare general coroutine stuff
        constexpr int           no_concurrency{1};
        boost::asio::io_context io_context{no_concurrency};
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) { io_context.stop(); });

        // Prepare server stuff
        const auto server_id{server::generateServerId()};
        auto       socket{boost::asio::ip::udp::socket(io_context, {boost::asio::ip::udp::v4(), port})};

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
                [&]() { return active_clients.getNumberOfClients(); }, controller_name_filter, mapping_file,
                sensor_auto_toggle, gamepad_data),
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
