#include "swissknifehelper.h"
#include <common/logger.h>
#include "integer.h"
#include "ifloat.h"

using namespace Jgv::GenICam;

void SwissKnifeHelper::initFormula(Poco::XML::Node *node,
                                   std::weak_ptr<GenICamXMLParser> xmlParser,
                                   std::weak_ptr<IPort::Interface> port,
                                   std::string const &formulaName)
{
    if (_formula) {
        return;
    }

    Poco::XML::Node *p = node->firstChild();
    for (; p; p = p->nextSibling()) {
        if (p->nodeType() != Poco::XML::Node::ELEMENT_NODE) {
            continue;
        }

        Poco::XML::Element *q = dynamic_cast<Poco::XML::Element*>(p);
        if (q->tagName() == "pVariable") {
            std::string name = q->getAttribute("Name");
            std::string val = q->firstChild()->nodeValue();
            std::shared_ptr<RegNode> node = RegNode::create(val, xmlParser, port);
            if (node->interfaceType() == Type::IInteger) {
                Integer::Interface *iface;
                iface = dynamic_cast<Integer::Interface*>(node->interface());
                _intVariables[name] = iface->getValue();
            } else if (node->interfaceType() == Type::IFloat) {
                Float::Interface *iface;
                iface = dynamic_cast<Float::Interface*>(node->interface());
                _floatVariables[name] = iface->getValue();
            }
        } else if (q->tagName() == formulaName) {
            _formula = std::make_shared<Formula>(q->firstChild()->nodeValue());
        }
    }
}

uint64_t SwissKnifeHelper::getValueInt()
{
    if (!_formula) {
        lyu_error("No formula");
        return 0;
    }
    uint64_t value = _formula->evaluateAsInteger(_intVariables, _floatVariables);
#ifdef _DEBUG
    _formula->debugOutput(std::string());
    lyu_debug("RESULT: " << value << "(0x" << std::hex << value << ")");
#endif
    return value;
}

double SwissKnifeHelper::getValueFloat()
{
    if (!_formula) {
        lyu_error("No formula");
        return 0;
    }
    double value = _formula->evaluateAsFloat(_intVariables, _floatVariables);
#ifdef _DEBUG
    _formula->debugOutput(std::string());
    lyu_debug("RESULT: " << value << "(0x" << std::hex << *(uint64_t*)&value << ")");
#endif
    return value;
}
