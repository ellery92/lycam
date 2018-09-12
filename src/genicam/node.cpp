#include "node.h"
#include <Poco/DOM/Element.h>

using namespace Jgv::GenICam;

Node::Node(std::string const &name,
           std::weak_ptr<GenICamXMLParser> xmlParser,
           std::weak_ptr<IPort::Interface> port)
{
    _xmlParser = xmlParser;
    auto p = _xmlParser.lock();
    poco_assert(p);
    _node = p->getNodeByNameAttribute(name);
    _port = port;
}

std::string Node::typeString()
{
    return _node->tagName();
}

std::string Node::getChildNodeValue(std::string const &name)
{
	Poco::XML::Node *p = _node->firstChild();
    while (p) {
        if (p->nodeName() == name)
            return p->firstChild()->nodeValue();
        p = p->nextSibling();
    }
    return std::string();
}

std::string Node::getNodeAttribute(std::string const &attr)
{
    return _node->getAttribute(attr);
}

std::string Node::toolTip()
{
    return getChildNodeValue("ToolTip");
}

std::string Node::description()
{
    return getChildNodeValue("Description");
}

std::string Node::displayName()
{
    return getChildNodeValue("DisplayName");
}

std::string Node::featureName()
{
    return getNodeAttribute("Name");
}

std::string Node::visibility()
{
    return getChildNodeValue("Visibility");
}

std::shared_ptr<Node>
Node::MakeNode(std::string const name,
               std::weak_ptr<GenICamXMLParser> xmlParser,
               std::weak_ptr<IPort::Interface> port)
{
    auto p = xmlParser.lock();
    poco_assert(p);
    Poco::XML::Element *node = p->getNodeByNameAttribute(name);
    std::string typeName = node->tagName();
#if 0
    if (typeName == "Interger") {
        return std::make_shared<IntegerNode>(name, xmlParser, port);
    } else if (typeName == "IntReg") {
        return std::make_shared<IntReg>(name, xmlParser, port);
    } else if (typeName == "IntSwissKnife") {
        return std::make_shared<IntSwissKnife>(name, xmlParser, port);
    }
#endif
    return NULL;
}
