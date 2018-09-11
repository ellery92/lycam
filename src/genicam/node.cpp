#include "node.h"
#include <Poco/DOM/Element.h>

using namespace Jgv::GenICam;

Node::Node(std::string const &name,
           std::weak_ptr<GenICamXMLParser> xmlParser,
           std::weak_ptr<IPort::Interface> port)
    : _name(name),
      _xmlParser(xmlParser),
      _port(port){}

Poco::XML::Element *Node::element()
{
    return element(_name);
}

Poco::XML::Element *Node::element(std::string const &name)
{
    auto p = _xmlParser.lock();
    if (!p)
        return NULL;

    Poco::XML::Element *elem = p->getNodeByNameAttribute(name);
    if (!elem)
        return NULL;

    return elem;
}

std::string Node::typeString()
{
    Poco::XML::Element *elem = element();
    if (!elem)
        return std::string();

    return elem->tagName();
}

std::string Node::getNodeValue(Poco::XML::Node *node, const std::string &name)
{
	Poco::XML::Node *p = node->firstChild();
    while (p) {
        if (p->nodeName() == name)
            return p->firstChild()->nodeValue();
        p = p->nextSibling();
    }
    return std::string();
}

std::string Node::getNodeValue(std::string const &name)
{
    Poco::XML::Element *elem = element();
    if (!elem)
        return std::string();

    return getNodeValue(elem, name);
}

std::string Node::getNodeAttribute(std::string const &attr)
{
    Poco::XML::Element *elem = element();
    if (!elem)
        return std::string();

    return elem->getAttribute(attr);
}

std::string Node::toolTip()
{
    return getNodeValue("ToolTip");
}

std::string Node::description()
{
    return getNodeValue("Description");
}

std::string Node::displayName()
{
    return getNodeValue("DisplayName");
}

std::string Node::featureName()
{
    return getNodeAttribute("Name");
}

std::string Node::visibility()
{
    return getNodeValue("Visibility");
}

void Node::getAddrLength(Poco::XML::Node *node, uint64_t &addr,
                   uint64_t &length, std::string &endianess)
{
    std::string straddr = getNodeValue(node, "Address");
    addr = std::stoull(straddr, 0, 16);

    std::string strlength = getNodeValue(node, "Length");
    length =  std::stoull(strlength);

    endianess = getNodeValue(node, "Endianess");
}
