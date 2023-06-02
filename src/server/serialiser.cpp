// class header include
#include "serialiser.h"

// system includes

// local includes
#include "common.h"

//--------------------------------------------------------------------------------------------------

namespace server
{
namespace
{
std::vector<std::uint8_t> finalizeResponse(const std::vector<std::uint8_t>& payload, std::uint32_t server_id,
                                           DsuMsgType msg_type)
{
    std::size_t               index{0};
    std::vector<std::uint8_t> data(20, 0);

    // Header
    writeUInt8(data, index, 'D');
    writeUInt8(data, index, 'S');
    writeUInt8(data, index, 'U');
    writeUInt8(data, index, 'S');
    writeUInt16LE(data, index, getProtocolVersion());
    writeUInt16LE(data, index, payload.size() + 4);
    writeUInt32LE(data, index, 0x00 /* Reserved for CRC32 */);
    writeUInt32LE(data, index, server_id);

    // Msg type (adds 4 bytes to size)
    writeUInt32LE(data, index, enumToValue(msg_type));

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
        writeUInt8(data, index, gamepad_data->m_sensor.m_ts != 0 ? 0x02 : 0x00 /* gyro state */);
        writeUInt8(data, index,
                   gamepad_data->m_battery == shared::details::BatteryLevel::Wired ? 0x01 : 0x02 /* connection type */);
    }
    else
    {
        writeUInt8(data, index, 0x00 /* disconnected */);
        writeUInt8(data, index, 0x00 /* no gyro yet */);
        writeUInt8(data, index, 0x00 /* no connection type yet */);
    }

    // This is a custom MAC address implementation, since SDL does not provide any. MAC is always mapped to the pad
    // index
    writeUInt8(data, index, 0x00);
    writeUInt8(data, index, 0x00);
    writeUInt8(data, index, 0x00);
    writeUInt8(data, index, 0x00);
    writeUInt8(data, index, 0x00);
    writeUInt8(data, index, pad_index);

    if (gamepad_data)
    {
        writeUInt8(data, index, enumToValue(gamepad_data->m_battery));
    }
    else
    {
        writeUInt8(data, index, 0x00 /* no battery yet */);
    }
}

//--------------------------------------------------------------------------------------------------

void serialiseButtonFlagsA(const shared::GamepadData& gamepad_data, std::vector<std::uint8_t>& data, std::size_t& index)
{
    std::uint8_t flag{0};
    if (gamepad_data.m_dpad.m_left)
        flag |= 0x80;
    if (gamepad_data.m_dpad.m_down)
        flag |= 0x40;
    if (gamepad_data.m_dpad.m_right)
        flag |= 0x20;
    if (gamepad_data.m_dpad.m_up)
        flag |= 0x10;

    if (gamepad_data.m_special.m_start)
        flag |= 0x08;
    if (gamepad_data.m_right_stick.m_pressed)
        flag |= 0x04;
    if (gamepad_data.m_left_stick.m_pressed)
        flag |= 0x02;
    if (gamepad_data.m_special.m_back)
        flag |= 0x01;

    writeUInt8(data, index, flag);
}

//--------------------------------------------------------------------------------------------------

void serialiseButtonFlagsB(const shared::GamepadData& gamepad_data, std::vector<std::uint8_t>& data, std::size_t& index)
{
    std::uint8_t flag{0};
    if (gamepad_data.m_abxy.m_x)
        flag |= 0x80;
    if (gamepad_data.m_abxy.m_a)
        flag |= 0x40;
    if (gamepad_data.m_abxy.m_b)
        flag |= 0x20;
    if (gamepad_data.m_abxy.m_y)
        flag |= 0x10;

    if (gamepad_data.m_shoulder.m_right)
        flag |= 0x08;
    if (gamepad_data.m_shoulder.m_left)
        flag |= 0x04;
    if (gamepad_data.m_trigger.m_right == 0xFF)
        flag |= 0x02;
    if (gamepad_data.m_trigger.m_left == 0xFF)
        flag |= 0x01;

    writeUInt8(data, index, flag);
}

//--------------------------------------------------------------------------------------------------

void serialiseTouch(const shared::details::Touch& touch, std::vector<std::uint8_t>& data, std::size_t& index)
{
    writeUInt8(data, index, touch.m_touched ? 0x01 : 0x00);
    writeUInt8(data, index, touch.m_id);
    writeUInt16LE(data, index, touch.m_x);
    writeUInt16LE(data, index, touch.m_y);
}
}  // namespace

//--------------------------------------------------------------------------------------------------

std::vector<std::uint8_t> serialise(const VersionResponse&, std::uint32_t server_id)
{
    std::size_t               index{0};
    std::vector<std::uint8_t> data(4, 0);

    // Payload
    writeUInt32LE(data, index, getProtocolVersion());

    return finalizeResponse(data, server_id, DsuMsgType::Version);
}

//--------------------------------------------------------------------------------------------------

std::vector<std::vector<std::uint8_t>> serialise(const ListPortsResponse& response, std::uint32_t server_id)
{
    std::vector<std::vector<std::uint8_t>> data_list;
    for (const auto pad_index : response.m_requested_indexes)
    {
        std::size_t               index{0};
        std::vector<std::uint8_t> data(12, 0);

        serialiseGamepadHeader(response.m_gamepad_data[pad_index], pad_index, data, index);
        writeUInt8(data, index, 0x00);  // Trailing 0 for some reason

        data_list.push_back(finalizeResponse(data, server_id, DsuMsgType::ListPorts));
    }

    return data_list;
}

//--------------------------------------------------------------------------------------------------

std::vector<std::uint8_t> serialise(const PadDataResponse& response, std::uint32_t server_id)
{
    std::size_t               index{0};
    std::vector<std::uint8_t> data(80, 0);

    serialiseGamepadHeader(response.m_gamepad_data, response.m_pad_index, data, index);

    writeUInt8(data, index, response.m_gamepad_data ? 0x01 : 0x00);
    writeUInt32LE(data, index, response.m_packet_counter);

    // In case we have no data, the rest of fields can be left zero-ed out
    if (response.m_gamepad_data)
    {
        const auto& gamepad_data{*response.m_gamepad_data};

        serialiseButtonFlagsA(gamepad_data, data, index);
        serialiseButtonFlagsB(gamepad_data, data, index);

        writeUInt8(data, index, gamepad_data.m_special.m_guide ? 0x01 : 0x00);
        writeUInt8(data, index, gamepad_data.m_touchpad.m_pressed ? 0x01 : 0x00);

        writeUInt8(data, index, gamepad_data.m_left_stick.m_x);
        writeUInt8(data, index, gamepad_data.m_left_stick.m_y);

        writeUInt8(data, index, gamepad_data.m_right_stick.m_x);
        writeUInt8(data, index, gamepad_data.m_right_stick.m_y);

        writeUInt8(data, index, gamepad_data.m_dpad.m_left ? 0xFF : 0x00);
        writeUInt8(data, index, gamepad_data.m_dpad.m_down ? 0xFF : 0x00);
        writeUInt8(data, index, gamepad_data.m_dpad.m_right ? 0xFF : 0x00);
        writeUInt8(data, index, gamepad_data.m_dpad.m_up ? 0xFF : 0x00);

        writeUInt8(data, index, gamepad_data.m_abxy.m_x ? 0xFF : 0x00);
        writeUInt8(data, index, gamepad_data.m_abxy.m_a ? 0xFF : 0x00);
        writeUInt8(data, index, gamepad_data.m_abxy.m_b ? 0xFF : 0x00);
        writeUInt8(data, index, gamepad_data.m_abxy.m_y ? 0xFF : 0x00);

        writeUInt8(data, index, gamepad_data.m_shoulder.m_right ? 0xFF : 0x00);
        writeUInt8(data, index, gamepad_data.m_shoulder.m_left ? 0xFF : 0x00);

        writeUInt8(data, index, gamepad_data.m_trigger.m_right);
        writeUInt8(data, index, gamepad_data.m_trigger.m_left);

        serialiseTouch(gamepad_data.m_touchpad.m_first_touch, data, index);
        serialiseTouch(gamepad_data.m_touchpad.m_second_touch, data, index);

        writeUInt64LE(data, index, gamepad_data.m_sensor.m_ts);
        writeFloatLE(data, index, gamepad_data.m_sensor.m_accel.m_x);
        writeFloatLE(data, index, gamepad_data.m_sensor.m_accel.m_y);
        writeFloatLE(data, index, gamepad_data.m_sensor.m_accel.m_z);
        writeFloatLE(data, index, gamepad_data.m_sensor.m_gyro.m_pitch);
        writeFloatLE(data, index, gamepad_data.m_sensor.m_gyro.m_yaw);
        writeFloatLE(data, index, gamepad_data.m_sensor.m_gyro.m_roll);
    }

    return finalizeResponse(data, server_id, DsuMsgType::PadData);
}
}  // namespace server
