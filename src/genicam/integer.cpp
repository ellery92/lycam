#include "integer.h"
#include <common/logger.h>
#include "intreg.h"
#include "intswissknife.h"

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
    RegNode::Ptr node = RegNode::create(cname, _xmlParser, _port);
    if (!node)
        return;

    if (node->interfaceType() == Type::IInteger) {
        dynamic_cast<Integer::Interface*>(node->interface())->setValue(value);
    }
}
