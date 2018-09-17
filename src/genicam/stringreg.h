#pragma once
#include "node.h"
#include "istring.h"

namespace Jgv {

namespace GenICam {

class StringRegNode final : public Node, public String::Interface
{
public:
    StringRegNode(std::string const &name, GenICamXMLParser::WeakPtr xmlParser,
                IPort::Interface::WeakPtr port)
        : Node(name, xmlParser, port) {}

    virtual ~StringRegNode() = default;
    virtual std::string getValue() override;
    virtual void setValue(const std::string &value) override;
    virtual uint64_t getMaxLength() override;

    virtual Interface *interface() override { return this; }
};

} // namespace GenICam

} // namespace Jgv
