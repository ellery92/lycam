#pragma once
#include "node.h"
#include "ifloat.h"

namespace Jgv {

namespace GenICam {

class FloatNode final : public Node, public Float::Interface
{
public:
    FloatNode(std::string const &name,
              GenICamXMLParser::WeakPtr xmlParser,
              IPort::Interface::WeakPtr port)
        : Node(name, xmlParser, port) {}

    virtual ~FloatNode() = default;
    virtual double getValue() override;
    virtual void setValue(double value) override;
    virtual double getMin() override;
    virtual double getMax() override;
    virtual double getInc() override;

    virtual Interface *interface() override { return this; }
};

} // namespace GenICam

} // namespace Jgv
