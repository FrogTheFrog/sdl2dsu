#pragma once

// system includes
#include <cstdint>
#include <set>
#include <vector>

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

struct PadDataResponse
{
    const std::uint8_t                        m_pad_index;
    const std::uint32_t                       m_client_id;
    const std::uint32_t                       m_packet_counter;
    const std::optional<shared::GamepadData>& m_gamepad_data;
};

//--------------------------------------------------------------------------------------------------

std::vector<std::uint8_t> serialise(const VersionResponse& response, std::uint32_t server_id);

//--------------------------------------------------------------------------------------------------

std::vector<std::vector<std::uint8_t>> serialise(const ListPortsResponse& response, std::uint32_t server_id);

//--------------------------------------------------------------------------------------------------

std::vector<std::uint8_t> serialise(const PadDataResponse& response, std::uint32_t server_id);
}  // namespace server
