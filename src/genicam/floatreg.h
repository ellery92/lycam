#pragma once
#include "ifloat.h"
#include "node.h"

namespace Jgv {

namespace GenICam {

class FloatRegNode final : public Node, public Float::Interface
{
public:
    FloatRegNode(std::string const &name,
                 GenICamXMLParser::WeakPtr xmlParser,
                 IPort::Interface::WeakPtr port)
        : Node(name, xmlParser, port) {}

    virtual ~FloatRegNode() = default;
    virtual double getValue() override;
    virtual void setValue(double value) override;
    virtual double getMin() override { return 0; }
    virtual double getMax() override { return 0; }
    virtual double getInc() override { return 0; }

    virtual Interface *interface() override { return this; }
};

} // namespace GenICam

} // namespace Jgv
