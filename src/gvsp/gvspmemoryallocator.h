/*  -*- mode: c++ -*- */
#ifndef GVSPMEMORYALLOCATOR_H
#define GVSPMEMORYALLOCATOR_H

#include <memory>
#include <iostream>

namespace Jgv
{

namespace Gvsp
{

struct MemoryBlockPrivate
{
};

class MemoryBlock
{
public:
    MemoryBlock();
    MemoryBlock(uint8_t *p);
    MemoryBlock(const MemoryBlock &other) = default;
    virtual ~MemoryBlock();
    uint8_t * data = nullptr;
protected:
    std::shared_ptr<MemoryBlockPrivate> impl;
};

struct Image;
class MemoryAllocator
{
public:
    virtual ~MemoryAllocator() = default;
    virtual MemoryBlock allocate(std::size_t size) noexcept;
    virtual void destroy(MemoryBlock &) noexcept;
    virtual void push(Image &) noexcept;
};

} // namespace Gvsp

} // namespace Jgv

#endif // GVSPMEMORYALLOCATOR_H
