#pragma once
#include "node.h"
#include "ifloat.h"

namespace Jgv {

namespace GenICam {

class FloatNode final : public Node, public Float::Interface
{
public:
    virtual ~FloatNode() = default;
    virtual double getValue() override;
    virtual void setValue(double value) override;
    virtual double getMin() override;
    virtual double getMax() override;
    virtual double getInc() override;

    Interface *interface() { return this; }

private:
    double getValue(std::string const &name, std::string const &pname);
};

} // namespace GenICam

} // namespace Jgv
