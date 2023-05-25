#pragma once

// system includes
#include <cstdint>
#include <vector>

// local includes

//--------------------------------------------------------------------------------------------------

namespace server
{
enum class DsuMsgType : std::uint32_t
{
    Version   = 0x100000,
    ListPorts = 0x100001,
    PadData   = 0x100002
};

//--------------------------------------------------------------------------------------------------

template<class T>
std::underlying_type_t<T> enumToValue(T enum_value)
{
    return static_cast<std::underlying_type_t<T>>(enum_value);
}

//--------------------------------------------------------------------------------------------------

std::uint16_t getProtocolVersion();

//--------------------------------------------------------------------------------------------------

std::uint8_t readUInt8(const std::vector<std::uint8_t>& data, std::size_t& index, bool shift_index = true);

//--------------------------------------------------------------------------------------------------

std::uint16_t readUInt16LE(const std::vector<std::uint8_t>& data, std::size_t& index, bool shift_index = true);

//--------------------------------------------------------------------------------------------------

std::uint32_t readUInt32LE(const std::vector<std::uint8_t>& data, std::size_t& index, bool shift_index = true);

//--------------------------------------------------------------------------------------------------

std::int32_t readInt32LE(const std::vector<std::uint8_t>& data, std::size_t& index, bool shift_index = true);

//--------------------------------------------------------------------------------------------------

void writeUInt8(std::vector<std::uint8_t>& data, std::size_t& index, std::uint8_t value);

//--------------------------------------------------------------------------------------------------

void writeUInt16LE(std::vector<std::uint8_t>& data, std::size_t& index, std::uint16_t value);

//--------------------------------------------------------------------------------------------------

void writeUInt32LE(std::vector<std::uint8_t>& data, std::size_t& index, std::uint32_t value);

//--------------------------------------------------------------------------------------------------

std::uint32_t calculateCrc32(const std::vector<std::uint8_t>& data);
}  // namespace server
