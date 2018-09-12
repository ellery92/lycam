#pragma once
#include "reg.h"
#include "iinteger.h"
#include <memory>

namespace Jgv {

namespace GenICam {

class IntegerNode : public RegNode, public Integer::Interface
{
public:
    IntegerNode(std::string const &name, std::weak_ptr<GenICamXMLParser> xmlParser,
                std::weak_ptr<IPort::Interface> port)
        : RegNode(name, xmlParser, port) {}

    virtual ~IntegerNode() = default;
    virtual uint64_t getValue() override;
    virtual void setValue(uint64_t value) override;
    virtual uint64_t getMin() override;
    virtual uint64_t getMax() override;
    virtual uint64_t getInc() override;

    virtual Interface *interface() override { return this; }
};

} // namespace GenICam

} // namespace Jgv
