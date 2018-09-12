#pragma once
#include "reg.h"
#include "iinteger.h"

namespace Jgv {

namespace GenICam {

class IntRegNode final : public RegNode, public Integer::Interface
{
public:
    IntRegNode(std::string const &name, std::weak_ptr<GenICamXMLParser> xmlParser,
               std::weak_ptr<IPort::Interface> port)
        : RegNode(name, xmlParser, port) {}

    virtual ~IntRegNode() = default;
    virtual uint64_t getValue() override;
    virtual void setValue(uint64_t value) override;
    virtual uint64_t getMin() override { return 0; }
    virtual uint64_t getMax() override { return 0; }
    virtual uint64_t getInc() override { return 0; };

    virtual Interface *interface() override { return this; }
};

} // namespace GenICam

} // namespace Jgv
