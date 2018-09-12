#include "floatreg.h"
#include <common/logger.h>

using namespace Jgv::GenICam;

double FloatRegNode::getValue()
{
    auto port = _port.lock();
    if (!port) {
        lyu_error("port is NULL");
        return 0;
    }

    float value = 0;
    uint64_t addr = getAddr();
    uint64_t length = getLength();
    port->read((uint8_t *)&value, addr, length);

    lyu_debug("read from " << std::hex << addr
              << "(" << length << "): " << std::dec << value);

    return value;
}

void FloatRegNode::setValue(double value)
{
    auto port = _port.lock();
    if (!port) {
        lyu_error("port is NULL");
        return;
    }

    float fval = value;
    uint64_t addr = getAddr();
    uint64_t length = getLength();
    lyu_debug("write " << value << " to " << std::hex << addr << "(" << length << ")");
    port->write((uint8_t *)&fval, addr, length);
}
