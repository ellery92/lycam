#include "integer.h"
#include <common/logger.h>

using namespace Jgv::GenICam;

uint64_t IntegerNode::getValue(std::string const &name, std::string const &pname)
{
    std::string strval = getNodeValue(name);
    if (!strval.empty()) {
        return stoull(strval);
    }

    Poco::XML::Element *node = element(getNodeValue(pname));
    if (!node) {
        return 0;
    }

    uint64_t value = 0;
    std::string tagName = node->tagName();
    if (tagName == "IntReg") {
        value = getValueIntReg(node);
    } else if (tagName == "IntSwissKnife") {
        value = getValueIntSwissknife(node);
    }

    return value;
}

uint64_t IntegerNode::getValueIntReg(Poco::XML::Node *node)
{
    uint64_t addr, length;
    std::string endianess;
    getAddrLength(node, addr, length, endianess);

    auto port = _port.lock();
    if (!port) {
        lyu_error("port is NULL");
        return 0;
    }

    uint64_t value = 0;
    port->read((uint8_t *)&value, addr, length);
    lyu_debug("read from " << std::hex << addr
              << "(" << length << "): " << std::dec << value);

    return value;
}

uint64_t IntegerNode::getValueIntSwissknife(Poco::XML::Node *node)
{
    return 0;
}

uint64_t IntegerNode::getValue()
{
    return getValue("Value", "pValue");
}

void IntegerNode::setValue(uint64_t value)
{
    Poco::XML::Element *node = element(getNodeValue("pValue"));
    if (!node)
        return;

    uint64_t addr, length;
    std::string endianess;
    getAddrLength(node, addr, length, endianess);

    auto port = _port.lock();
    if (!port) {
        lyu_error("port is NULL");
        return;
    }

    uint8_t *pvalue;
    uint32_t value32 = value;
    if (length == 4) {
        pvalue = (uint8_t*)&value32;
    } else {
        pvalue = (uint8_t*)&value;
    }
    lyu_debug("write " << value << " to " << std::hex << addr << "(" << length << ")");
    port->write((uint8_t *)pvalue, addr, length);
}

uint64_t IntegerNode::getMin()
{
    return getValue("Min", "pMin");
}

uint64_t IntegerNode::getMax()
{
    return getValue("Max", "pMax");
}

uint64_t IntegerNode::getInc()
{
    return getValue("Inc", "pInc");
}
