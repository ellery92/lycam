/* -*- mode: c++ -*- */
#ifndef TIMESTAMPDATE_H
#define TIMESTAMPDATE_H

#include "gvspreceiver.h"

namespace Jgv
{

namespace Gvsp
{

class TimestampDate
{
public:
    using Ptr = std::shared_ptr<TimestampDate>;
    static Ptr create();

    virtual ~TimestampDate() = default;

    virtual void push(uint64_t timestamp, uint64_t dateMin, uint64_t dateMax) noexcept = 0;
    virtual uint64_t getDate(uint64_t timestamp) noexcept = 0;
}; // clas TimestampDate

} // namespace Gvcp

} // namespace Jgv

#endif // TIMESTAMPDATE_H
