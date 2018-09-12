#pragma once
#include "ifloat.h"
#include "reg.h"

namespace Jgv {

namespace GenICam {

class FloatRegNode final : public RegNode, public Float::Interface
{
public:
    FloatRegNode(std::string const &name,
              std::weak_ptr<GenICamXMLParser> xmlParser,
              std::weak_ptr<IPort::Interface> port)
        : RegNode(name, xmlParser, port) {}

    virtual ~FloatRegNode() = default;
    virtual double getValue() override;
    virtual void setValue(double value) override;
    virtual double getMin() override { return 0; }
    virtual double getMax() override { return 0; }
    virtual double getInc() override { return 0; }

    Interface *interface() { return this; }
};

} // namespace GenICam

} // namespace Jgv
