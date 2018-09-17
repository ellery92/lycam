#include "timestampdate.h"
#include "timestampdate_p.h"

#include <cmath>

using namespace Jgv::Gvsp;

TimestampDate::Ptr TimestampDate::create()
{
  return std::make_shared<TimestampDatePrivate>();
}

void TimestampDatePrivate::updateByRegression()
{
    auto it = datas.cbegin();
    if (it == datas.cend()) {
        return;
    }

    long double xsomme = 0;
    long double ysomme = 0;

    // on fait une translation de repère, pour ne pas déborder
    const long double x0 = (*it).timestamp;
    const long double y0 = (*it).min;

    for (auto const &it: datas) {
        const long double x = it.timestamp - x0;
        const long double y1 = it.min - y0;
        const long double y2 = it.max - y0;
        xsomme += x;
        ysomme += (y1 + y2);
    }

    const long double moyX = xsomme / datas.size();
    const long double moyY = ysomme / (datas.size() * 2);

    long double nu = 0;
    long double de = 0;

    for (auto const &it: datas) {
        const long double x = it.timestamp - x0;
        const long double y1 = it.min - y0;
        const long double y2 = it.max - y0;

        nu += (x - moyX) / k * (y1 - moyY) / k;
        nu += (x - moyX) / k * (y2 - moyY) / k;
        de += ((x - moyX) / k * (x - moyX) / k);
        de += ((x - moyX) / k * (x - moyX) / k);
    }



    const long double aa = nu / de;
    const long double bb = moyY - aa * static_cast<long double>(moyX);

    std::lock_guard<std::mutex> lock(read_mutex);
    a = aa;
    b = bb;
    x = x0;
    y = y0;
}

void TimestampDatePrivate::push(uint64_t timestamp,
                                uint64_t dateMin, uint64_t dateMax) noexcept
{
    std::lock_guard<std::mutex> lock(write_mutex);
    // on filtre, en supprimant les données dont l'écart de mesure est le plus grand
    while (datas.size() > 15) {
        auto it = datas.cbegin();
        auto toRemove = it;
        int delta = 0;

        for (; it != datas.cend(); ++it) {
            const int d = static_cast<int>((*it).max) - static_cast<int>((*it).min);
            if (d > delta) {
                delta = d;
                toRemove = it;
            }
        }
        datas.erase(toRemove);
    }

    datas.push_back(TimeData {timestamp, dateMin, dateMax});

    if (datas.size() > 2) {
        updateByRegression();
    }
}

uint64_t TimestampDatePrivate::getDate(uint64_t timestamp) noexcept
{
    std::lock_guard<std::mutex> lock(read_mutex);
    long double ts = (static_cast<long double>(timestamp) - x) * a;
    ts += (y + b);

    return static_cast<uint64_t>(std::llround(ts));
}
