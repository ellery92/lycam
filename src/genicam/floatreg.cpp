#include "floatreg.h"
#include <util/logger.h>

using namespace Jgv::GenICam;

double FloatRegNode::getValue()
{
    auto port = _port.lock();
    if (!port) {
        lyu_error("port is NULL");
        return 0;
    }

    float value = 0;
    uint64_t addr = address();
    uint64_t len = length();
    port->read((uint8_t *)&value, addr, len);

    lyu_debug("read from " << std::hex << addr
              << "(" << len << "): " << std::dec << value);

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
    uint64_t addr = address();
    uint64_t len = length();
    lyu_debug("write " << value << " to " << std::hex << addr << "(" << len << ")");
    port->write((uint8_t *)&fval, addr, len);
}
