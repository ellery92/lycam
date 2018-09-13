#include "floatjgv.h"
#include "logger.h"
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
    Node::Ptr node = Node::create(cname, _xmlParser, _port);
    if (!node)
        return;

    Float::Interface *iface = dynamic_cast<Float::Interface*>(node->interface());
    if (iface)
        iface->setValue(value);
}
