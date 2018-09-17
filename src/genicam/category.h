#pragma once
#include "node.h"
#include "icategory.h"

namespace Jgv {

namespace GenICam {

class CategoryNode final : public Node, public Category::Interface
{
public:
    CategoryNode(std::string const &name,
                 std::weak_ptr<GenICamXMLParser> xmlParser,
                 std::weak_ptr<IPort::Interface> port)
        : Node(name, xmlParser, port) {}

    virtual ~CategoryNode() = default;

    virtual Interface *interface() override {
        return this;
    }
};

} // namespace GenICam

} // namespace Jgv
