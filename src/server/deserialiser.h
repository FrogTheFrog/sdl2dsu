#pragma once

// system includes
#include <cstdint>
#include <optional>
#include <set>
#include <variant>
#include <vector>

// local includes

//--------------------------------------------------------------------------------------------------

namespace server
{
struct VersionRequest
{
};

//--------------------------------------------------------------------------------------------------

struct ListPortsRequest
{
    std::set<std::uint8_t> m_requested_indexes;
};

//--------------------------------------------------------------------------------------------------

struct PadDataRequest
{
    std::uint32_t               m_client_id;
    std::optional<std::uint8_t> m_requested_index;
};

//--------------------------------------------------------------------------------------------------

std::optional<std::variant<VersionRequest, ListPortsRequest, PadDataRequest>>
    deserialise(std::vector<std::uint8_t> data);
}  // namespace server
