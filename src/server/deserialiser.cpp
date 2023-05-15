// class header include
#include "deserialiser.h"

// system includes
#include <boost/log/trivial.hpp>

// local includes
#include "common.h"

//--------------------------------------------------------------------------------------------------

namespace server
{
namespace
{
const std::size_t MIN_DATA_SIZE{8 /* Data size up to and including the packet size bytes */};

//--------------------------------------------------------------------------------------------------

std::optional<ListPortsRequest> deserialiseListPorts(std::vector<std::uint8_t>& data, std::size_t& index)
{
    const auto request_size{readInt32LE(data, index)};
    if (request_size < 0 || request_size > 4)
    {
        BOOST_LOG_TRIVIAL(trace) << "DSU Client packet received with invalid request size: " << request_size;
        return std::nullopt;
    }

    std::set<std::uint8_t> requested_indexes;
    for (auto i = 0; i < request_size; ++i)
    {
        const auto req_index{readUInt8(data, index)};
        if (req_index > 3)
        {
            BOOST_LOG_TRIVIAL(trace) << "DSU Client packet received with invalid request index: " << req_index;
            return std::nullopt;
        }

        requested_indexes.insert(req_index);
    }

    return ListPortsRequest{std::move(requested_indexes)};
}

//--------------------------------------------------------------------------------------------------

std::optional<PadDataRequest> deserialisePadData(std::uint32_t client_id, std::vector<std::uint8_t>& data,
                                                 std::size_t& index)
{
    std::set<std::uint8_t> requested_indexes;
    const auto             req_flag{readUInt8(data, index)};

    std::uint8_t req_index{readUInt8(data, index)};
    if (req_flag & 0x01 /* slot based registration */)
    {
        if (req_index > 3)
        {
            BOOST_LOG_TRIVIAL(trace) << "DSU Client packet received with invalid request index: " << req_index;
            return std::nullopt;
        }

        requested_indexes.insert(req_index);
    }

    // This is custom MAC address handling where the first byte corresponds to the slot index
    std::uint8_t mac_index{readUInt8(data, index)};
    if (req_flag & 0x02 /* MAC based registration */)
    {
        if (mac_index <= 3)
        {
            requested_indexes.insert(mac_index);
        }
    }

    return PadDataRequest{client_id, requested_indexes};
}
}  // namespace

//--------------------------------------------------------------------------------------------------

std::optional<std::variant<VersionRequest, ListPortsRequest, PadDataRequest>>
    deserialise(std::vector<std::uint8_t> data)
{
    if (data.size() < MIN_DATA_SIZE)
    {
        BOOST_LOG_TRIVIAL(trace) << "Got data packet of size: " << data.size();
        return std::nullopt;
    }

    std::size_t index{0};
    if (readUInt8(data, index) != 'D' ||  //
        readUInt8(data, index) != 'S' ||  //
        readUInt8(data, index) != 'U' ||  //
        readUInt8(data, index) != 'C')
    {
        BOOST_LOG_TRIVIAL(trace) << "Got non-DSU Client related packet of size: " << data.size();
        return std::nullopt;
    }

    const auto protocol_version{readUInt16LE(data, index)};
    if (protocol_version != getProtocolVersion())
    {
        BOOST_LOG_TRIVIAL(trace) << "DSU Client requested protocol version: " << protocol_version << ". Ours is "
                                 << getProtocolVersion();
        return std::nullopt;
    }

    const auto packet_size{readUInt16LE(data, index)};
    if (packet_size + MIN_DATA_SIZE > data.size())
    {
        BOOST_LOG_TRIVIAL(trace) << "DSU Client packet received with size mismatch: " << packet_size + MIN_DATA_SIZE
                                 << ". We got " << data.size();
        return std::nullopt;
    }

    const auto packet_crc32{readUInt32LE(data, index, false)};
    writeUInt32LE(data, index, 0u);  // Clear the CRC so that it can be calculated

    const auto calculated_crc{calculateCrc32(data)};
    if (packet_crc32 != calculated_crc)
    {
        BOOST_LOG_TRIVIAL(trace) << "DSU Client packet crc32 mismatch: " << packet_crc32 << ". We got "
                                 << calculated_crc;
        return std::nullopt;
    }

    const auto client_id = readUInt32LE(data, index);
    const auto msg_type  = readUInt32LE(data, index);

    if (msg_type == enumToValue(DsuMsgType::Version))
    {
        BOOST_LOG_TRIVIAL(trace) << "DSU Client " << client_id << " requested server version.";
        return VersionRequest{};
    }
    else if (msg_type == enumToValue(DsuMsgType::ListPorts))
    {
        BOOST_LOG_TRIVIAL(trace) << "DSU Client " << client_id << " requested available ports.";
        return deserialiseListPorts(data, index);
    }
    else if (msg_type == enumToValue(DsuMsgType::PadData))
    {
        BOOST_LOG_TRIVIAL(trace) << "DSU Client " << client_id << " requested pad data.";
        return deserialisePadData(client_id, data, index);
    }

    BOOST_LOG_TRIVIAL(trace) << "DSU Client packet contains unhandle msg type " << msg_type << " from client "
                             << client_id;
    return std::nullopt;
}
}  // namespace server
