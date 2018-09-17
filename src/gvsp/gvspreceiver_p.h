/* -*- mode: c++ -*- */
#ifndef GVSPRECEIVER_P_H
#define GVSPRECEIVER_P_H

#include "gvspreceiver.h"
#include "gvspblock.h"
#include "gvspdevices.h"
#include "timestampdate.h"

#include "gvsp.h"

#ifdef ENDAT
#include "endat.h"
#endif

#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <time.h>

namespace Jgv {

namespace Gvsp {

using BlocksMap = std::map<unsigned, Block>;

class Packet;
class MemoryAllocator;
struct BlockDesc;

struct Clock {
    TimestampSource src = TimestampSource::GEVTransmitter;
    uint64_t UtcOffset = UINT64_C(37000000000);
};

class ReceiverPrivate final : public Receiver
{
public:
    ReceiverPrivate(MemoryAllocator *allocator);
    virtual ~ReceiverPrivate() = default;

    virtual void listenUnicast(uint32_t bindAddress) override;
    virtual void listenMulticast(uint32_t bindAddress, uint32_t multicastAddress) override;
    virtual void acceptFrom(uint32_t transmitterIP, uint16_t transmitterPort) override;
    virtual void preallocImages(const Geometry &geometry, uint32_t packetSize) override;
    virtual void setTimestampSource(TimestampSource source) override;
    virtual void setTransmitterTimestampFrequency(uint64_t frequency) override;
    virtual void setResendActive(bool active) override;
    virtual void pushDatation(uint64_t timestamp, uint64_t dateMin, uint64_t dateMax) override;

    virtual const Parameters &parameters() const override;
    virtual const ReceiverStatistics & statistics() const override;

#ifdef ENDAT
    virtual std::weak_ptr<Endat> getEndat() override;
#endif

private:
    const std::unique_ptr<MemoryAllocator> allocatorPtr;

    int sd = -1;
    int raw = -1;
    Parameters params {0,0,0,0,0,SocketType::NoType,TimestampSource::GEVTransmitter,1000000000,true,false,{0,0,0}};

    volatile bool run = true;                  // contrôle la sortie de boucle
    std::unique_ptr<std::thread> threadPtr;

    std::mutex mutex;
    std::condition_variable cond_variable;

    Clock clock; // gestion de l'horloge

#ifdef ENDAT
    std::shared_ptr<Endat> endatPtr {new Endat};
#endif
    ReceiverStatistics statistics = {0,0,0,0,0};

    BlocksMap blocksMap;

    TimestampDate datation;

    void userStack();
    void ringStack();
    void loop();

    inline void doBuffer(const uint8_t *buffer, std::size_t size);
    inline void doBlock(const BlockDesc *block);
    inline void handleLeader(const Gvsp::Packet &gvsp);
    inline void handlePayload(const Gvsp::Packet &gvsp);
    inline void handleTrailer(const Gvsp::Packet &gvsp);

    inline uint64_t updateTimestamp(const Packet &gvsp);

    inline void callResend(uint16_t blockId, uint32_t firstId, uint32_t lastId);
    inline void doResend(uint16_t blockId, Block &block, uint32_t packetId);


}; // ReceiverPrivate

} // namespace Gvsp

} // namespace Jgv

#endif // GVSPRECEIVER_P_H
