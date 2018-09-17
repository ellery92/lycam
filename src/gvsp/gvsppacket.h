/* -*- mode: c++ -*- */
#ifndef GVSPPACKET_H
#define GVSPPACKET_H

#include <stdint.h>
#include <cstddef>

namespace Jgv
{

namespace Gvsp
{

struct Header;
struct DataLeader;
struct DataLeaderImage;
struct PayloadImage;
struct DATATRAILER;
struct DataTrailerImage;

union pData {
    const uint8_t *p;
    const Header *header;
    const DataLeader *leader;
    const DataLeaderImage *leaderImage;
    const PayloadImage *payload;
    const DATATRAILER *trailer;
    const DataTrailerImage *trailerImage;
}; // union pData

const uint32_t GVSP_HEADER_SIZE = 8;

class Packet final
{
    const pData m_data;
    const std::size_t m_size = 0;

public:
    explicit Packet(const uint8_t *const data, std::size_t size);
    Packet(const Packet &) = delete;
    Packet & operator=(const Packet &) = delete;

    uint16_t headerStatus() const noexcept;
    uint16_t headerBlockId() const noexcept;
    uint8_t headerPacketFormat() const noexcept;
    uint32_t headerPacketId() const noexcept;
    uint16_t leaderPayloadType() const noexcept;
    uint32_t leaderTimestampHigh() const noexcept;
    uint32_t leaderTimestampLow() const noexcept;
    uint32_t leaderImagePixelFormat() const noexcept;
    uint32_t leaderImageSizeX() const noexcept;
    uint32_t leaderImageSizeY() const noexcept;
    uint16_t leaderImagePaddingX() const noexcept;
    uint16_t leaderImagePaddingY() const noexcept;
    const uint8_t *payloadImageData() const noexcept;
    std::size_t imageDataSize() const noexcept;

    uint16_t trailerPayloadType() const noexcept;
    uint32_t trailerImageSizeY() const noexcept;

}; // class Packet

} // namespace Gvsp

} // namespace Jgv
#endif // GVSPPACKET_H
