#pragma once

#include "node.h"
#include "ifloat.h"

namespace Jgv {

namespace GenICam {

class SwissKnifeNode final : public Node, public Float::Interface
{
public:
    SwissKnifeNode(std::string const &name,
                   GenICamXMLParser::WeakPtr xmlParser,
                   IPort::Interface::WeakPtr port)
        : Node(name, xmlParser, port) {}

    virtual ~SwissKnifeNode() = default;
    virtual double getValue() override;
    virtual void setValue(double value) override;
    virtual double getMin() override { return 0; }
    virtual double getMax() override { return 0; }
    virtual double getInc() override { return 0; }

    virtual Interface *interface() override { return this; }
}; // class Object

} // namespace GenICam

} // namespace Jgv
