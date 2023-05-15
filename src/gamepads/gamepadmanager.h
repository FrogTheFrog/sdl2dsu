#pragma once

// system includes
#include <boost/log/trivial.hpp>
#include <map>
#include <regex>
#include <set>

// local includes
#include "gamepadhandle.h"
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
class GamepadManager final
{
    BOOST_MOVABLE_BUT_NOT_COPYABLE(GamepadManager)

public:
    explicit GamepadManager(std::regex controller_name_filter, shared::GamepadDataContainer& gamepad_data);

    std::optional<std::uint8_t> tryOpenGamepad(std::uint32_t id);
    std::optional<std::uint8_t> closeGamepad(std::uint32_t id);
    void                        tryChangeSensorState(std::uint32_t id, const std::optional<bool>& enable);
    void                        tryChangeSensorStateForAll(const std::optional<bool>& enable);

    template<class UpdateFunction>
    std::optional<std::uint8_t> tryUpdateData(std::uint32_t id, UpdateFunction update_function);

private:
    std::regex                             m_controller_name_filter;
    std::set<std::uint32_t>                m_pending_ids;
    std::map<std::uint32_t, GamepadHandle> m_open_handles;
    shared::GamepadDataContainer&          m_gamepad_data;
};

//--------------------------------------------------------------------------------------------------

template<class UpdateFunction>
std::optional<std::uint8_t> GamepadManager::tryUpdateData(std::uint32_t id, UpdateFunction update_function)
{
    auto handle_it{m_open_handles.find(id)};
    if (handle_it == std::end(m_open_handles))
    {
        BOOST_LOG_TRIVIAL(error) << "gamepad with id " << id << " has not data!";
        return std::nullopt;
    }

    const auto index{handle_it->second.getIndex()};
    auto&      data{m_gamepad_data[index]};
    BOOST_ASSERT(data);

    return update_function(*data) ? std::make_optional(index) : std::nullopt;
}
}  // namespace gamepads
