#include "enumeration.h"
#include "enumentry.h"
#include "iinteger.h"
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Node.h>
using namespace Jgv::GenICam;

std::string Jgv::GenICam::EnumerationNode::getStringValue()
{
    uint64_t value = getIntValue();
    Enumeration::Interface::EntryList entries = getEntries();
    for (auto &entry: entries) {
        if (entry->getValue() == value)
            return entry->getName();
    }
    return std::string();
}

void Jgv::GenICam::EnumerationNode::setStringValue(const std::string& value) {
    Enumeration::Interface::EntryList entries = getEntries();
    for (auto &entry: entries) {
        if (entry->getName() == value)
            setIntValue(entry->getValue());
    }
}

uint64_t Jgv::GenICam::EnumerationNode::getIntValue()
{
    return getValueInt("Value");
}

void Jgv::GenICam::EnumerationNode::setIntValue(uint64_t value) {
    std::string cname = getChildNodeValue("pValue");
    Node::Ptr node = Node::create(cname, _xmlParser, _port);
    if (!node)
        return;

    Integer::Interface *iface = dynamic_cast<Integer::Interface*>(node->interface());
    if (iface)
        iface->setValue(value);
}

Enumeration::Interface::EntryList Jgv::GenICam::EnumerationNode::getEntries() const {
    Enumeration::Interface::EntryList b;
    Poco::XML::Node *node = _node->firstChild();
    for (; node; node = node->nextSibling()) {
        if (node->nodeName() == "EnumEntry") {
            Poco::XML::Element *elem = dynamic_cast<Poco::XML::Element*>(node);
            if (elem) {
                std::string name = elem->getAttribute("Name");
                b.push_back(std::make_shared<EnumEntryNode>(name, _xmlParser, _port));
            }
        }
    }

    return b;
}
