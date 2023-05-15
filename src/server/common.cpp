// class header include
#include "common.h"

// system includes
#include <boost/crc.hpp>

// local includes

//--------------------------------------------------------------------------------------------------

namespace server
{
std::uint16_t getProtocolVersion()
{
    return 1001;
}

//--------------------------------------------------------------------------------------------------

std::uint8_t readUInt8(const std::vector<std::uint8_t>& data, std::size_t& index, bool shift_index)
{
    BOOST_ASSERT(index < data.size());

    const std::uint8_t first{data[index]};

    index += shift_index ? 1 : 0;
    return first;
}

//--------------------------------------------------------------------------------------------------

std::uint16_t readUInt16LE(const std::vector<std::uint8_t>& data, std::size_t& index, bool shift_index)
{
    BOOST_ASSERT(index + 1 < data.size());

    const std::uint8_t first{data[index]};
    const std::uint8_t second{data[index + 1]};

    index += shift_index ? 2 : 0;
    return first | second << 8;
}

//--------------------------------------------------------------------------------------------------

std::uint32_t readUInt32LE(const std::vector<std::uint8_t>& data, std::size_t& index, bool shift_index)
{
    BOOST_ASSERT(index + 3 < data.size());

    const std::uint8_t first{data[index]};
    const std::uint8_t second{data[index + 1]};
    const std::uint8_t third{data[index + 2]};
    const std::uint8_t fourth{data[index + 3]};

    index += shift_index ? 4 : 0;
    return first | second << 8 | third << 16 | fourth << 24;
}

//--------------------------------------------------------------------------------------------------

std::int32_t readInt32LE(const std::vector<std::uint8_t>& data, std::size_t& index, bool shift_index)
{
    return static_cast<std::int32_t>(readUInt32LE(data, index, shift_index));
}

//--------------------------------------------------------------------------------------------------

void writeUInt8(std::vector<std::uint8_t>& data, std::size_t& index, std::uint8_t value)
{
    BOOST_ASSERT(index < data.size());

    data[index++] = value;
}

//--------------------------------------------------------------------------------------------------

void writeUInt16LE(std::vector<std::uint8_t>& data, std::size_t& index, std::uint16_t value)
{
    BOOST_ASSERT(index + 1 < data.size());

    data[index++] = value;
    data[index++] = value >> 8;
}

//--------------------------------------------------------------------------------------------------

void writeUInt32LE(std::vector<std::uint8_t>& data, std::size_t& index, std::uint32_t value)
{
    BOOST_ASSERT(index + 3 < data.size());

    data[index++] = value;
    data[index++] = value >> 8;
    data[index++] = value >> 16;
    data[index++] = value >> 24;
}

//--------------------------------------------------------------------------------------------------

void writeUInt64LE(std::vector<std::uint8_t>& data, std::size_t& index, std::uint64_t value)
{
    BOOST_ASSERT(index + 7 < data.size());

    data[index++] = value;
    data[index++] = value >> 8;
    data[index++] = value >> 16;
    data[index++] = value >> 24;
    data[index++] = value >> 32;
    data[index++] = value >> 40;
    data[index++] = value >> 48;
    data[index++] = value >> 56;
}

//--------------------------------------------------------------------------------------------------

void writeFloatLE(std::vector<std::uint8_t>& data, std::size_t& index, float value)
{
    static_assert(sizeof(std::uint32_t) == sizeof(float));
    const auto ptr_cast{reinterpret_cast<std::uint32_t*>(&value)};
    writeUInt32LE(data, index, *ptr_cast);
}

//--------------------------------------------------------------------------------------------------

std::uint32_t calculateCrc32(const std::vector<std::uint8_t>& data)
{
    boost::crc_32_type crc;
    crc.process_bytes(data.data(), data.size());
    return crc.checksum();
}
}  // namespace server
