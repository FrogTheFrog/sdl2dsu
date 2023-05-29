// class header include
#include "gamepadmanager.h"

// system includes

// local includes

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
namespace
{
std::optional<std::uint8_t> findFreeIndex(const shared::GamepadDataContainer& gamepad_data)
{
    auto it = std::find_if(std::begin(gamepad_data), std::end(gamepad_data),
                           [](const auto& data) { return data == std::nullopt; });
    if (it == std::end(gamepad_data))
    {
        return std::nullopt;
    }

    return it - std::begin(gamepad_data);
}
}  // namespace

//--------------------------------------------------------------------------------------------------

GamepadManager::GamepadManager(std::regex controller_name_filter, shared::GamepadDataContainer& gamepad_data)
    : m_controller_name_filter{std::move(controller_name_filter)}
    , m_gamepad_data{gamepad_data}
{
}

//--------------------------------------------------------------------------------------------------

std::optional<std::uint8_t> GamepadManager::tryOpenGamepad(std::uint32_t id)
{
    BOOST_ASSERT(!m_open_handles.contains(id));

    m_pending_ids.erase(id);
    if (m_open_handles.size() == 4)
    {
        m_pending_ids.insert(id);
        return std::nullopt;
    }

    const auto index{findFreeIndex(m_gamepad_data)};
    BOOST_ASSERT(index);

    // Emplacing is required first due to no-copy-ctor
    const auto result{m_open_handles.try_emplace(id, id, *index)};
    BOOST_ASSERT(result.second);
    const auto& handle{result.first->second};

    if (handle.getHandle() == nullptr)
    {
        m_open_handles.erase(id);
        return std::nullopt;
    }

    if (!std::regex_search(handle.getName(), m_controller_name_filter))
    {
        BOOST_LOG_TRIVIAL(info) << handle.getName() << " does not match the filter.";
        m_open_handles.erase(id);
        return std::nullopt;
    }

    m_gamepad_data[*index] = shared::GamepadData{};
    return index;
}

//--------------------------------------------------------------------------------------------------

std::optional<std::uint8_t> GamepadManager::closeGamepad(std::uint32_t id)
{
    m_pending_ids.erase(id);

    auto open_handle_it{m_open_handles.find(id)};
    if (open_handle_it == std::end(m_open_handles))
    {
        return std::nullopt;
    }

    const auto index{open_handle_it->second.getIndex()};
    m_gamepad_data[open_handle_it->second.getIndex()] = std::nullopt;
    m_open_handles.erase(open_handle_it);

    std::set<std::uint32_t> current_pending_ids;
    std::swap(m_pending_ids, current_pending_ids);

    for (const auto& pending_id : current_pending_ids)
    {
        const auto new_index{tryOpenGamepad(pending_id)};
        if (new_index)
        {
            BOOST_ASSERT(*new_index == index);
        }
    }

    return index;
}

//--------------------------------------------------------------------------------------------------

void GamepadManager::tryChangeSensorState(std::uint32_t id, const std::optional<bool>& enable)
{
    auto open_handle_it{m_open_handles.find(id)};
    if (open_handle_it != std::end(m_open_handles))
    {
        open_handle_it->second.tryChangeSensorState(enable);
    }
}
}  // namespace gamepads
