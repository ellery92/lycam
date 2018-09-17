#ifndef GVSPBLOCK_H
#define GVSPBLOCK_H

#include "gvspdevices.h"

#include <vector>

namespace Jgv {

namespace Gvsp
{

struct PtrInfos;


enum class SegmentState : uint64_t /*assure le padding*/ {empty,askedAgain,filled};

struct BlockStatistics {
    long unsigned empty = 0;
    long unsigned askedAgain = 0;
};

class Block final
{

public:
    Block();
    ~Block();

    // pas d'affectation
    Block & operator=(const Block &) = delete;

    Image & image() noexcept;
    const Image & image() const noexcept;
    const Geometry & geometry() const noexcept;
    BlockStatistics segmentState;

    std::size_t segmentSize() const noexcept;
    uint32_t endID() const noexcept;

    bool changeGeometry(const Geometry &geometry) noexcept;
    void mapMemory(std::size_t segmentSize) noexcept;

    void insertSegment(unsigned packetId, const uint8_t * const segment, std::size_t segmentSize) noexcept;

    void setLeaderIsFilled() noexcept ;
    bool leaderIsFilled() const noexcept;

    void setLeaderAskedAgain() noexcept ;
    void setSegmentsAskedAgain(std::size_t startID, std::size_t stopID) noexcept;

    unsigned emptySegmentsCount(unsigned packetId) const noexcept;

    //void dump() const;

private:
    std::vector<PtrInfos> m_memoryMapping;  // le tableau des addresses des pointeurs de segments
    Image m_image;                          // structure contenant les données images
    SegmentState m_leaderState = SegmentState::empty;             // l'état du pointeur du Leader GVSP

}; // class Block

} // namespace Gvsp

} // namespace Jgv

#endif // GVSPBLOCK_H
