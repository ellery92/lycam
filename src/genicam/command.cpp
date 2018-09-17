#include "command.h"
#include "iinteger.h"

using namespace Jgv::GenICam;

void CommandNode::execute()
{
    std::string cname = getChildNodeValue("pValue");
    Node::Ptr node = Node::create(cname, _xmlParser, _port);
    if (!node)
        return;

    Integer::Interface *iface = dynamic_cast<Integer::Interface*>(node->interface());
    uint64_t value = getValueInt("CommandValue");
    if (iface)
        iface->setValue(value);
}
