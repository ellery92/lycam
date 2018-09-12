#include "converter.h"
#include <common/logger.h>
#include "swissknifehelper.h"
#include "iinteger.h"
#include "ifloat.h"

using namespace Jgv::GenICam;

double ConverterNode::getValue()
{
    // read pvalue
    std::string cname = getChildNodeValue("pValue");
    RegNode::Ptr node = RegNode::create(cname, _xmlParser, _port);
    if (!node)
        return 0;

    SwissKnifeHelper swissKnife;
    swissKnife.initFormula(_node, _xmlParser, _port, "FormulaFrom");

    if (node->interfaceType() == Type::IInteger) {
        uint64_t v = dynamic_cast<Integer::Interface*>(node->interface())->getValue();
        swissKnife.addIntVariable("TO", v);
        lyu_debug("converter pvalue int: " << v << " 0x" << std::hex << v);
    } else if (node->interfaceType() == Type::IFloat) {
        double v = dynamic_cast<Float::Interface*>(node->interface())->getValue();
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
    RegNode::Ptr node = RegNode::create(cname, _xmlParser, _port);
    if (!node)
        return;

    SwissKnifeHelper swissKnife;
    swissKnife.initFormula(_node, _xmlParser, _port, "FormulaTo");
    swissKnife.addFloatVariable("FROM", value);

    if (node->interfaceType() == Type::IInteger) {
        uint64_t v = swissKnife.getValueInt();
        dynamic_cast<Integer::Interface*>(node->interface())->setValue(v);
    } else if (node->interfaceType() == Type::IFloat) {
        double v = swissKnife.getValueFloat();
        dynamic_cast<Integer::Interface*>(node->interface())->setValue(v);
    }
}
