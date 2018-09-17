#include "integer.h"

using namespace Jgv::GenICam;

uint64_t IntegerNode::getValue()
{
    return getValueInt("Value");
}

uint64_t IntegerNode::getMin()
{
    return getValueInt("Min");
}

uint64_t IntegerNode::getMax()
{
    return getValueInt("Max");
}

uint64_t IntegerNode::getInc()
{
    return getValueInt("Inc");
}

void IntegerNode::setValue(uint64_t value)
{
    std::string cname = getChildNodeValue("pValue");
    Node::Ptr node = Node::create(cname, _xmlParser, _port);
    if (!node)
        return;

    Integer::Interface *iface = dynamic_cast<Integer::Interface*>(node->interface());
    if (iface)
        iface->setValue(value);
}
