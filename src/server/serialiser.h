#pragma once

// system includes
#include <cstdint>
#include <vector>
#include <set>

// local includes
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace server
{
struct VersionResponse
{
};

//--------------------------------------------------------------------------------------------------

struct ListPortsResponse
{
    const std::set<std::uint8_t>&       m_requested_indexes;
    const shared::GamepadDataContainer& m_gamepad_data;
};

//--------------------------------------------------------------------------------------------------

std::vector<std::uint8_t> serialise(const VersionResponse& response, std::uint32_t server_id);

//--------------------------------------------------------------------------------------------------

std::vector<std::vector<std::uint8_t>> serialise(const ListPortsResponse& response, std::uint32_t server_id);
}  // namespace server
