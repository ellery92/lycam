#pragma once

#include "reg.h"
#include "ifloat.h"

namespace Jgv {

namespace GenICam {

class ConverterNode final : public RegNode, public Float::Interface
{
public:
    ConverterNode(std::string const &name,
                  std::weak_ptr<GenICamXMLParser> xmlParser,
                  std::weak_ptr<IPort::Interface> port)
        : RegNode(name, xmlParser, port) {}

    virtual ~ConverterNode() = default;
    virtual double getValue() override;
    virtual void setValue(double value) override;
    virtual double getMin() override { return 0; }
    virtual double getMax() override { return 0; }
    virtual double getInc() override { return 0; }

    Interface *interface() { return this; }
}; // class Object

} // namespace GenICam

} // namespace Jgv
