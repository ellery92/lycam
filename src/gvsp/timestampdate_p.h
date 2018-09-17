/* -*- mode: c++ -*- */
#ifndef TIMESTAMPDATE_P_H
#define TIMESTAMPDATE_P_H

#include <mutex>
#include <list>

namespace Jgv
{

namespace Gvsp
{

struct TimeData {
    uint64_t timestamp;
    uint64_t min;
    uint64_t max;
};

class TimestampDatePrivate final : public TimestampDate
{
public:
    virtual void push(uint64_t timestamp, uint64_t dateMin, uint64_t dateMax) noexcept override;
    virtual uint64_t getDate(uint64_t timestamp) noexcept override;
    virtual ~TimestampDatePrivate() = default;

private:
    long double a = 1;
    long double b = 0;
    long double x = 0;
    long double y = 0;

    const long double k = 1024;
    std::list<TimeData> datas;

    std::mutex read_mutex;
    std::mutex write_mutex;
    void updateByRegression();

}; // class TimestampDatePrivate

} // namespace Gvsp

} // namespace Jgv

#endif // TIMESTAMPDATE_P_H
