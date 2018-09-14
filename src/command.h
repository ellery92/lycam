#pragma once
#include "node.h"
#include "icommand.h"

namespace Jgv {

namespace GenICam {

class CommandNode final : public Node, public Command::Interface
{
public:
    CommandNode(std::string const &name,
         std::weak_ptr<GenICamXMLParser> xmlParser,
         std::weak_ptr<IPort::Interface> port)
        : Node(name, xmlParser, port) {}

    virtual ~CommandNode() = default;

    virtual Interface *interface() override {
        return this;
    }

    // GenICam
    void execute() override;
};

} // namespace GenICam

} // namespace Jgv
