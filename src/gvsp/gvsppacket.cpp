#include "gvsppacket.h"

#include <boost/endian/conversion.hpp>

using namespace Jgv::Gvsp;
using namespace boost::endian;

struct Jgv::Gvsp::Header {
    uint16_t status;
    uint16_t blockId;
    uint32_t packetFormat_packetId;
};

//struct GVSP_HEADER_EXT {
//    unsigned char blockId64HighPart[4];
//    unsigned char blockId64LowPart[4];
//    unsigned char packetId32[4];
//};

struct Jgv::Gvsp::DataLeader {
    Header header;
    uint16_t reserved;
    uint16_t payloadType;
    uint32_t timestampHighPart;
    uint32_t timestampLowPart;
};

struct Jgv::Gvsp::DataLeaderImage {
    DataLeader leader;
    uint32_t pixelFormat;
    uint32_t sizeX;
    uint32_t sizeY;
    uint32_t offsetX;
    uint32_t offsetY;
    uint16_t paddingX;
    uint16_t paddingY;
};

struct Jgv::Gvsp::PayloadImage {
    Header header;
    unsigned char data[4];  // fixe la taille de façon arbitraire (conformité c++2011)
};

struct Jgv::Gvsp::DATATRAILER {
    Header header;
    uint16_t reserved;
    uint16_t payloadType;
};

struct Jgv::Gvsp::DataTrailerImage {
    DATATRAILER trailer;
    uint32_t sizeY;
};

Packet::Packet(const uint8_t *const data, std::size_t size)
    : m_data{data}, m_size{size}
{}

uint16_t Packet::headerStatus() const noexcept
{
    return big_to_native(m_data.header->status);
}

uint16_t Packet::headerBlockId() const noexcept
{
    return big_to_native(m_data.header->blockId);
}

uint8_t Packet::headerPacketFormat() const noexcept
{
    return UINT8_C(0xFF) & (big_to_native(m_data.header->packetFormat_packetId)>>24);
}

uint32_t Packet::headerPacketId() const noexcept
{
    return UINT32_C(0x00FFFFFF) & big_to_native(m_data.header->packetFormat_packetId);
}

uint16_t Packet::leaderPayloadType() const noexcept
{
    return big_to_native(m_data.leader->payloadType);
}

uint32_t Packet::leaderTimestampHigh() const noexcept
{
    return big_to_native(m_data.leader->timestampHighPart);
}

uint32_t Packet::leaderTimestampLow() const noexcept
{
    return big_to_native(m_data.leader->timestampLowPart);
}

uint32_t Packet::leaderImagePixelFormat() const noexcept
{
    return big_to_native(m_data.leaderImage->pixelFormat);
}

uint32_t Packet::leaderImageSizeX() const noexcept
{
    return big_to_native(m_data.leaderImage->sizeX);
}

uint32_t Packet::leaderImageSizeY() const noexcept
{
    return big_to_native(m_data.leaderImage->sizeY);
}

uint16_t Packet::leaderImagePaddingX() const noexcept
{
    return big_to_native(m_data.leaderImage->paddingX);
}

uint16_t Packet::leaderImagePaddingY() const noexcept
{
    return big_to_native(m_data.leaderImage->paddingY);
}

const uint8_t *Packet::payloadImageData() const noexcept
{
    return m_data.payload->data;
}

std::size_t Packet::imageDataSize() const noexcept
{
    return m_size - sizeof(Header);
}

uint16_t Packet::trailerPayloadType() const noexcept
{
    return big_to_native(m_data.trailer->payloadType);
}

uint32_t Packet::trailerImageSizeY() const noexcept
{
    return big_to_native(m_data.trailerImage->sizeY);
}
