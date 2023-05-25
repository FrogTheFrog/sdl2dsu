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

GamepadManager::GamepadManager(shared::GamepadDataContainer& gamepad_data)
    : m_gamepad_data{gamepad_data}
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

    if (result.first->second.getHandle() == nullptr)
    {
        m_open_handles.erase(id);
        return std::nullopt;
    }

    m_gamepad_data[*index]                       = shared::GamepadData{};
    m_gamepad_data[*index]->m_sensor.m_supported = result.first->second.hasSensorSupport();

    return index;
}

//--------------------------------------------------------------------------------------------------

std::uint8_t GamepadManager::closeGamepad(std::uint32_t id)
{
    m_pending_ids.erase(id);

    auto open_handle_it{m_open_handles.find(id)};
    BOOST_ASSERT(open_handle_it != std::end(m_open_handles));

    const auto index{open_handle_it->second.getIndex()};
    m_gamepad_data[open_handle_it->second.getIndex()] = std::nullopt;
    m_open_handles.erase(open_handle_it);

    std::set<std::uint32_t> current_pending_ids;
    std::swap(m_pending_ids, current_pending_ids);

    for (const auto& pending_id : m_pending_ids)
    {
        const auto new_index{tryOpenGamepad(pending_id)};
        if (new_index)
        {
            BOOST_ASSERT(*new_index == index);
        }
    }

    return index;
}
}  // namespace gamepads
