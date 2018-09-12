#include "intreg.h"
#include <common/logger.h>

using namespace Jgv::GenICam;

uint64_t IntRegNode::getValue()
{
    auto port = _port.lock();
    if (!port) {
        lyu_error("port is NULL");
        return 0;
    }

    uint64_t addr = getAddr();
    uint64_t length = getLength();
    uint64_t value = 0;
    port->read((uint8_t *)&value, addr, length);

    lyu_debug("read from " << std::hex << addr
              << "(" << length << "): " << std::dec << value);

    return value;
}

void IntRegNode::setValue(uint64_t value)
{
    auto port = _port.lock();
    if (!port) {
        lyu_error("port is NULL");
        return;
    }

    uint8_t *pvalue;
    uint32_t value32 = value;
    uint64_t addr = getAddr();
    uint64_t length = getLength();
    if (length == 4) {
        pvalue = (uint8_t*)&value32;
    } else {
        pvalue = (uint8_t*)&value;
    }
    lyu_debug("write " << value << " to " << std::hex << addr << "(" << length << ")");
    port->write((uint8_t *)pvalue, addr, length);
}
