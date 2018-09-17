#include "converter.h"
#include "swissknifehelper.h"
#include "iinteger.h"
#include "ifloat.h"
#include <Poco/DOM/Node.h>
#include <Poco/DOM/Element.h>
#include <util/logger.h>

using namespace Jgv::GenICam;

double ConverterNode::getValue()
{
    // read pvalue
    std::string cname = getChildNodeValue("pValue");
    Node::Ptr node = Node::create(cname, _xmlParser, _port);
    if (!node)
        return 0;

    SwissKnifeHelper swissKnife;
    swissKnife.initFormula(_node, _xmlParser, _port, "FormulaFrom");

    GenICam::Interface *giface = node->interface();
    if (giface->interfaceType() == Type::IInteger) {
        uint64_t v = dynamic_cast<Integer::Interface*>(giface)->getValue();
        swissKnife.addIntVariable("TO", v);
        lyu_debug("converter pvalue int: " << v << " 0x" << std::hex << v);
    } else if (giface->interfaceType() == Type::IFloat) {
        double v = dynamic_cast<Float::Interface*>(giface)->getValue();
        swissKnife.addFloatVariable("TO", v);
        lyu_debug("converter pvalue float: " << v);
    }
    double value = swissKnife.getValueFloat();
    return value;
}

void ConverterNode::setValue(double value)
{
    // read pvalue
    std::string cname = getChildNodeValue("pValue");
    Node::Ptr node = Node::create(cname, _xmlParser, _port);
    if (!node)
        return;

    SwissKnifeHelper swissKnife;
    swissKnife.initFormula(_node, _xmlParser, _port, "FormulaTo");
    swissKnife.addFloatVariable("FROM", value);

    if (node->interface()->interfaceType() == Type::IInteger) {
        uint64_t v = swissKnife.getValueFloat();
        lyu_debug("set value ->converter pvalue int: " << v);
        dynamic_cast<Integer::Interface*>(node->interface())->setValue(v);
    } else if (node->interface()->interfaceType() == Type::IFloat) {
        double v = swissKnife.getValueFloat();
        lyu_debug("set value ->converter pvalue float: " << v);
        dynamic_cast<Integer::Interface*>(node->interface())->setValue(v);
    }
}
