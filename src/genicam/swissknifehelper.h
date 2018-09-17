#pragma once
#include "formula.h"
#include "node.h"

namespace Jgv {

namespace GenICam {

class SwissKnifeHelper final
{
public:
    void initFormula(Poco::XML::Node *node,
                     GenICamXMLParser::WeakPtr xmlParser,
                     IPort::Interface::WeakPtr port,
                     std::string const &formulaName = "Formula");

    uint64_t getValueInt();
    double getValueFloat();

    void addIntVariable(std::string const name, uint64_t value) {
        _intVariables[name] = value;
    }

    void addFloatVariable(std::string const name, double value) {
        _floatVariables[name] = value;
    }

private:
    IntegerVariables _intVariables;
    FloatVariables _floatVariables;
    std::shared_ptr<Formula> _formula;
}; // class Object

} // namespace GenICam

} // namespace Jgv
