#include "floatjgv.h"
#include <common/logger.h>

using namespace Jgv::GenICam;

double FloatNode::getValue(std::string const &name, std::string const &pname)
{
    std::string strval = getNodeValue(name);
    if (!strval.empty()) {
        return stod(strval);
    }

    uint64_t addr, length;
    std::string endianess;
    getAddrLength(pname, addr, length, endianess);

    if (endianess.empty()) {
        lyu_warning("no property: " << name << " " << pname);
        return 0;
    }

    auto port = _port.lock();
    if (!port) {
        lyu_error("port is NULL");
        return 0;
    }

    if (length != 4) {
        lyu_warning("floatNode length = " << length);
        length = 4; // for safe
    }

    float value;
    port->read((uint8_t *)&value, addr, length);
    uint32_t *pval = (uint32_t*)&value;
    *pval = toNative(*pval, endianess);

    lyu_debug("read " << name << "from " << std::hex << addr << "(" << length << "): "
              << std::dec << value);

    return value;
}

double FloatNode::getValue()
{
    return getValue("Value", "pValue");
}

double FloatNode::getMin()
{
    return getValue("Min", "pMin");
}

double FloatNode::getMax()
{
    return getValue("Max", "pMax");
}

double FloatNode::getInc()
{
    return getValue("Inc", "pInc");
}

void FloatNode::setValue(double value)
{
    uint64_t addr, length;
    std::string endianess;
    getAddrLength("pValue", addr, length, endianess);

    auto port = _port.lock();
    if (!port) {
        lyu_error("port is NULL");
        return;
    }

    float fvalue = value;
    lyu_debug("write " << value << " to " << std::hex << addr << "(" << length << ")");
    port->write((uint8_t *)&fvalue, addr, length);
}
