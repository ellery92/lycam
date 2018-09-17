#include "gvspmemoryallocator.h"

#include <memory>

using namespace Jgv::Gvsp;

struct DefaultMemoryBlockPrivate : public MemoryBlockPrivate
{
    DefaultMemoryBlockPrivate(std::size_t size)
        : dataPtr(new uint8_t[size], std::default_delete<uint8_t[]>())
    {}
    std::shared_ptr<uint8_t> dataPtr;
};

class DefaultMemoryBlock : public MemoryBlock
{
public:
    DefaultMemoryBlock(std::size_t size)
    {
        impl = std::make_shared<DefaultMemoryBlockPrivate>(size);
        data = static_cast<DefaultMemoryBlockPrivate *>(impl.get())->dataPtr.get();
    }
    virtual ~DefaultMemoryBlock();

};
DefaultMemoryBlock::~DefaultMemoryBlock() = default;

MemoryBlock::MemoryBlock()
{}

MemoryBlock::MemoryBlock(uint8_t *p)
    : data(p)
{}

MemoryBlock::~MemoryBlock()
{}


MemoryBlock MemoryAllocator::allocate(std::size_t size) noexcept
{
    return DefaultMemoryBlock(size);
}

void MemoryAllocator::destroy(MemoryBlock &/*block*/) noexcept
{}

void MemoryAllocator::push(Image &/*image*/) noexcept
{}
