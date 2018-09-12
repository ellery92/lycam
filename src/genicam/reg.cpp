#include "reg.h"
#include "integer.h"
#include "intreg.h"
#include "intswissknife.h"
#include "floatjgv.h"
#include "floatreg.h"
#include "swissknife.h"
#include "converter.h"

#include <common/logger.h>

using namespace Jgv::GenICam;

RegNode::Ptr RegNode::create(std::string const &name,
                             std::weak_ptr<GenICamXMLParser> xmlParser,
                             std::weak_ptr<IPort::Interface> port)
{
    auto p = xmlParser.lock();
    poco_assert(p);
    Poco::XML::Element *node = p->getNodeByNameAttribute(name);
    std::string typeName = node->tagName();
    if (typeName == "Interger") {
        return std::make_shared<IntegerNode>(name, xmlParser, port);
    } else if (typeName == "IntReg") {
        return std::make_shared<IntRegNode>(name, xmlParser, port);
    } else if (typeName == "IntSwissKnife") {
        return std::make_shared<IntSwissKnifeNode>(name, xmlParser, port);
    } else if (typeName == "Float") {
        return std::make_shared<FloatNode>(name, xmlParser, port);
    } else if (typeName == "FloatReg") {
        return std::make_shared<FloatRegNode>(name, xmlParser, port);
    } else if (typeName == "SwissKnife") {
        return std::make_shared<SwissKnifeNode>(name, xmlParser, port);
    } else if (typeName == "Converter") {
        return std::make_shared<ConverterNode>(name, xmlParser, port);
    }
    return NULL;
}

uint64_t RegNode::getValueInt(std::string const &name)
{
    std::string value = getChildNodeValue(name);
    if (!value.empty()) {
        return std::stoull(value, 0, 16);
    }

    std::string pvalue = getChildNodeValue(std::string("p") + name);
    RegNode::Ptr node = RegNode::create(pvalue, _xmlParser, _port);
    if (node->interfaceType() == Type::IInteger) {
        return dynamic_cast<Integer::Interface*>(node->interface())->getValue();
    }
    return 0;
}

double RegNode::getValueFloat(std::string const &name)
{
    std::string value = getChildNodeValue(name);
    if (!value.empty()) {
        return std::stod(value);
    }

    std::string pvalue = getChildNodeValue(std::string("p") + name);
    RegNode::Ptr node = RegNode::create(pvalue, _xmlParser, _port);
    if (node->interfaceType() == Type::IFloat) {
        return dynamic_cast<Float::Interface*>(node->interface())->getValue();
    }
    return 0;
}

uint64_t RegNode::getAddr()
{
    return getValueInt("Address");
}

uint64_t RegNode::getLength()
{
    return getValueInt("Length");
}

Type RegNode::interfaceType()
{
    return interface()->interfaceType();
}
