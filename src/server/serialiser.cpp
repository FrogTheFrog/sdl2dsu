// class header include
#include "serialiser.h"

// system includes
#include <boost/log/trivial.hpp>

// local includes
#include "common.h"

//--------------------------------------------------------------------------------------------------

namespace server
{
namespace
{
std::vector<std::uint8_t> finalizeResponse(const std::vector<std::uint8_t>& payload, std::uint32_t server_id)
{
    std::size_t               index{0};
    std::vector<std::uint8_t> data{16, 0};

    // Header
    writeUInt8(data, index, 'D');
    writeUInt8(data, index, 'S');
    writeUInt8(data, index, 'U');
    writeUInt8(data, index, 'S');
    writeUInt16LE(data, index, getProtocolVersion());
    writeUInt16LE(data, index, payload.size());
    writeUInt32LE(data, index, 0x00 /* Reserved for CRC32 */);
    writeUInt32LE(data, index, server_id);

    // Payload
    std::copy(std::begin(payload), std::end(payload), std::back_inserter(data));

    // Calculate CRC32
    index = 8;
    writeUInt32LE(data, index, calculateCrc32(data));

    return data;
}

//--------------------------------------------------------------------------------------------------

void serialiseGamepadHeader(const std::optional<shared::GamepadData>& gamepad_data, std::uint8_t pad_index,
                            std::vector<std::uint8_t>& data, std::size_t& index)
{
    writeUInt8(data, index, pad_index);
    if (gamepad_data)
    {
        writeUInt8(data, index, 0x02 /* connected */);
        writeUInt8(data, index, gamepad_data->m_sensor.m_supported ? 0x02 : 0x00 /* gyro state */);
        writeUInt8(data, index,
                   gamepad_data->m_battery == shared::details::BatteryLevel::Wired ? 0x01 : 0x02 /* connection type */);
    }
    else
    {
        writeUInt8(data, index, 0x00 /* disconnected */);
        writeUInt8(data, index, 0x00 /* no gyro yet */);
        writeUInt8(data, index, 0x00 /* no connection type yet */);
    }

    // MAC address is not supported
    writeUInt8(data, index, 0x00);
    writeUInt8(data, index, 0x00);
    writeUInt8(data, index, 0x00);
    writeUInt8(data, index, 0x00);
    writeUInt8(data, index, 0x00);
    writeUInt8(data, index, 0x00);

    if (gamepad_data)
    {
        writeUInt8(data, index, enumToValue(gamepad_data->m_battery));
    }
    else
    {
        writeUInt8(data, index, 0x00 /* no battery yet */);
    }
}
}  // namespace

//--------------------------------------------------------------------------------------------------

std::vector<std::uint8_t> serialise(const VersionResponse&, std::uint32_t server_id)
{
    std::size_t               index{0};
    std::vector<std::uint8_t> data{4, 0};

    // Payload
    writeUInt32LE(data, index, getProtocolVersion());

    return finalizeResponse(data, server_id);
}

//--------------------------------------------------------------------------------------------------

std::vector<std::vector<std::uint8_t>> serialise(const ListPortsResponse& response, std::uint32_t server_id)
{
    std::vector<std::vector<std::uint8_t>> data_list;
    for (const auto pad_index : response.m_requested_indexes)
    {
        std::size_t               index{0};
        std::vector<std::uint8_t> data{12, 0};

        serialiseGamepadHeader(response.m_gamepad_data[pad_index], pad_index, data, index);
        writeUInt8(data, index, 0x00);  // Trailing 0 for some reason

        data_list.push_back(finalizeResponse(data, server_id));
    }

    return data_list;
}

}  // namespace server
