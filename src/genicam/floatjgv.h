#pragma once
#include "reg.h"
#include "ifloat.h"

namespace Jgv {

namespace GenICam {

class FloatNode : public RegNode, public Float::Interface
{
public:
    FloatNode(std::string const &name,
                std::weak_ptr<GenICamXMLParser> xmlParser,
                std::weak_ptr<IPort::Interface> port)
        : RegNode(name, xmlParser, port) {}

    virtual ~FloatNode() = default;
    virtual double getValue() override;
    virtual void setValue(double value) override;
    virtual double getMin() override;
    virtual double getMax() override;
    virtual double getInc() override;

    Interface *interface() { return this; }
};

} // namespace GenICam

} // namespace Jgv
