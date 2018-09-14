#include "node.h"
#include "category.h"
#include "command.h"
#include "integer.h"
#include "intreg.h"
#include "intswissknife.h"
#include "floatjgv.h"
#include "floatreg.h"
#include "swissknife.h"
#include "converter.h"
#include "boolean.h"
#include "stringreg.h"
#include "enumeration.h"

#include <Poco/DOM/Element.h>
#include "logger.h"

using namespace Jgv::GenICam;

Node::Ptr Node::create(std::string const &name,
                       std::weak_ptr<GenICamXMLParser> xmlParser,
                       std::weak_ptr<IPort::Interface> port)
{
#define create_node(type) std::make_shared<type>(name, xmlParser, port)
    auto p = xmlParser.lock();
    poco_assert(p);
    Poco::XML::Element *node = p->getNodeByNameAttribute(name);
    std::string typeName = node->tagName();
    if (typeName == "Integer") {
        return create_node(IntegerNode);
    } else if (typeName == "IntReg") {
        return create_node(IntRegNode);
    } else if (typeName == "IntSwissKnife") {
        return create_node(IntSwissKnifeNode);
    } else if (typeName == "Float") {
        return create_node(FloatNode);
    } else if (typeName == "FloatReg") {
        return create_node(FloatRegNode);
    } else if (typeName == "SwissKnife") {
        return create_node(SwissKnifeNode);
    } else if (typeName == "Converter") {
        return create_node(ConverterNode);
    } else if (typeName == "Boolean") {
        return create_node(BooleanNode);
    } else if (typeName == "StringReg") {
        return create_node(StringRegNode);
    } else if (typeName == "Enumeration") {
        return create_node(EnumerationNode);
    } else if (typeName == "Command") {
        return create_node(CommandNode);
    }
    lyu_error("Not supported type: " << typeName);
    return NULL;
}

Node::Node(std::string const &name,
           GenICamXMLParser::WeakPtr xmlParser,
           IPort::Interface::WeakPtr port)
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

uint64_t Node::address()
{
    return getValueInt("Address");
}

uint64_t Node::length()
{
    return getValueInt("Length");
}

uint64_t Node::getValueInt(std::string const &name)
{
    std::string value = getChildNodeValue(name);
    if (!value.empty()) {
        return std::stoull(value, NULL, 0);
    }

    std::string pvalue = getChildNodeValue(std::string("p") + name);
    Node::Ptr node = Node::create(pvalue, _xmlParser, _port);

    Integer::Interface *iface = dynamic_cast<Integer::Interface*>(node->interface());
    if (iface)
        return iface->getValue();
    return 0;
}

double Node::getValueFloat(std::string const &name)
{
    std::string value = getChildNodeValue(name);
    if (!value.empty()) {
        return std::stod(value);
    }

    std::string pvalue = getChildNodeValue(std::string("p") + name);
    Node::Ptr node = Node::create(pvalue, _xmlParser, _port);
    Float::Interface *iface = dynamic_cast<Float::Interface*>(node->interface());
    if (iface)
        return iface->getValue();
    return 0;
}
