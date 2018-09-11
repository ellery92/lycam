#pragma once
#include "node.h"
#include "iinteger.h"

namespace Jgv {

namespace GenICam {

class IntegerNode final : public Node, public Integer::Interface
{
public:
    IntegerNode(std::string const &name,
                std::weak_ptr<GenICamXMLParser> xmlParser,
                std::weak_ptr<IPort::Interface> port)
        : Node(name, xmlParser, port) {}

    virtual ~IntegerNode() = default;
    virtual uint64_t getValue() override;
    virtual void setValue(uint64_t value) override;
    virtual uint64_t getMin() override;
    virtual uint64_t getMax() override;
    virtual uint64_t getInc() override;

    Interface *interface() override { return this; }

private:
    uint64_t getValue(std::string const &name, std::string const &pname);
    uint64_t getValueIntReg(Poco::XML::Node *node);
    uint64_t getValueIntSwissknife(Poco::XML::Node *node);
};

} // namespace GenICam

} // namespace Jgv
