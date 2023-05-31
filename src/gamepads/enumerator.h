#pragma once

// system includes
#include <boost/asio/awaitable.hpp>
#include <functional>
#include <regex>
#include <set>

// local includes
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
boost::asio::awaitable<void>
    enumerateAndWatch(const std::function<boost::asio::awaitable<void>(const std::set<std::uint8_t>&)>& notify_clients,
                      const std::function<std::size_t()>& get_number_of_active_clients,
                      const std::regex& controller_name_filter, const std::string& mapping_file,
                      bool sensor_auto_toggle, shared::GamepadDataContainer& gamepad_data);
}  // namespace gamepads
