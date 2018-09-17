/* -*- mode: c++ -*- */
#ifndef GVSPRECEIVER_H
#define GVSPRECEIVER_H

#include "gvspmemoryallocator.h"

#include <memory>

#ifdef ENDAT
class Endat;
#endif

namespace Jgv
{

namespace Gvsp
{

enum class SocketType
{
    NoType,
    Classic,
    RingBuffer,
};

enum class TimestampSource : int
{
    GEVTransmitter = 0,
    TimestampDate = 1,
    SystemClock = 2
};

struct Geometry
{
    uint32_t width;
    uint32_t height;
    uint32_t pixelFormat;
    inline bool operator !=(const Geometry &other) const noexcept {return (other.width!=width)||(other.height!=height)||(other.pixelFormat!=pixelFormat);}
    inline bool operator ==(const Geometry &other) const noexcept {return (other.width==width)&&(other.height==height)&&(other.pixelFormat==pixelFormat);}
};

struct Image
{
    Geometry geometry;
    std::size_t geometrySize;
    uint64_t timestamp;
#ifdef ENDAT
    int32_t site;
    int32_t gisement;
#endif
    std::size_t dataSize;
    Jgv::Gvsp::MemoryBlock memory;
};

struct ReceiverStatistics
{
    uint64_t imagesCount;
    uint64_t imagesLostCount;
    uint64_t segmentsResendCount;
    uint64_t segmentsLostCount;
    uint64_t lastTimestamp;
};

struct Parameters {
    uint32_t receiverIP;
    uint32_t transmitterIP;
    uint32_t multicastGroup;
    uint16_t receiverPort;
    uint16_t transmitterPort;
    SocketType socketType;
    TimestampSource timestampSrc;
    uint64_t timestampFrequency;
    bool resend;
    bool isMulticast;
    Geometry geometry;
};

class MemoryAllocator;
class Receiver
{
public:
    using Ptr = std::shared_ptr<Receiver>;
    static Ptr create();

public:
    virtual ~Receiver() = default;

    virtual void listenUnicast(uint32_t bindAddress) = 0;
    virtual void listenMulticast(uint32_t bindAddress, uint32_t multicastAddress) = 0;
    virtual void acceptFrom(uint32_t transmitterIP, uint16_t transmitterPort) = 0;
    virtual void preallocImages(const Geometry &geometry, uint32_t packetSize) = 0;
    virtual void setTimestampSource(TimestampSource source) = 0;
    virtual void setTransmitterTimestampFrequency(uint64_t frequency) = 0;
    virtual void setResendActive(bool active) = 0;
    virtual void pushDatation(uint64_t timestamp, uint64_t dateMin, uint64_t dateMax) = 0;

    virtual const Parameters &parameters() const = 0;
    virtual const ReceiverStatistics & statistics() const = 0;

#ifdef ENDAT
    virtual std::weak_ptr<Endat> getEndat() = 0;
#endif

protected:
    MemoryAllocator &allocator();
    const MemoryAllocator &allocator() const;
}; // class Receiver

} // namespace Gvsp

} // namespace Jgv

#endif // GVSPRECEIVER_H
