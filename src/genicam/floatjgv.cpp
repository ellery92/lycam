#include "floatjgv.h"
#include <common/logger.h>
#include "floatreg.h"
#include "swissknife.h"

using namespace Jgv::GenICam;

double FloatNode::getValue()
{
    return getValueFloat("Value");
}

double FloatNode::getMin()
{
    return getValueFloat("Min");
}

double FloatNode::getMax()
{
    return getValueFloat("Max");
}

double FloatNode::getInc()
{
    return getValueFloat("Inc");
}

void FloatNode::setValue(double value)
{
    std::string cname = getChildNodeValue("pValue");
    RegNode::Ptr node = RegNode::create(cname, _xmlParser, _port);
    if (!node)
        return;

    if (node->interfaceType() == Type::IFloat) {
        dynamic_cast<Float::Interface*>(node->interface())->setValue(value);
    }
}
