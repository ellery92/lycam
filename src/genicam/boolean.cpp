#include "boolean.h"
#include "iinteger.h"

using namespace Jgv::GenICam;

bool BooleanNode::getValue()
{
    uint64_t value = getValueInt("Value");
    uint64_t onvalue = getValueInt("OnValue");
    return (value == onvalue);
}

void BooleanNode::setValue(bool value)
{
    uint64_t onvalue = getValueInt("OnValue");
    uint64_t offvalue = getValueInt("OffValue");
    uint64_t value64 = value ? onvalue : offvalue;

    std::string cname = getChildNodeValue("pValue");
    Node::Ptr node = Node::create(cname, _xmlParser, _port);
    if (!node)
        return;

    if (node->interface()->interfaceType() == Type::IInteger) {
        dynamic_cast<Integer::Interface*>(node->interface())->setValue(value64);
    }
}
