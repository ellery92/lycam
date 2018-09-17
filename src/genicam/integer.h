#pragma once
#include "node.h"
#include "iinteger.h"
#include <memory>

namespace Jgv {

namespace GenICam {

class IntegerNode final : public Node, public Integer::Interface
{
public:
    IntegerNode(std::string const &name, GenICamXMLParser::WeakPtr xmlParser,
                IPort::Interface::WeakPtr port)
        : Node(name, xmlParser, port) {}

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
