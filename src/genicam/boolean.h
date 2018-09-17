#pragma once
#include "node.h"
#include "iboolean.h"

namespace Jgv {

namespace GenICam {

class BooleanNode final : public Node, public Boolean::Interface
{
public:
    BooleanNode(std::string const &name, GenICamXMLParser::WeakPtr xmlParser,
                IPort::Interface::WeakPtr port)
        : Node(name, xmlParser, port) {}

    bool getValue() override;
    void setValue(bool value) override;
    virtual Interface *interface() override { return this; }
};

} // namespace GenICam

} // namespace Jgv
