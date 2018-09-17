/* Local Variables: */
/* mode: c++ */
/* End: */

#pragma once
#include "node.h"
#include <memory>

namespace Jgv {

namespace GenICam {

class EnumEntryNode final : public Node
{
public:
    using Ptr = std::shared_ptr<EnumEntryNode>;

    EnumEntryNode(Poco::XML::Element *node, GenICamXMLParser::WeakPtr xmlParser,
                  IPort::Interface::WeakPtr port)
        : Node(node, xmlParser, port) {}

    virtual ~EnumEntryNode() = default;
    uint64_t getValue();
    std::string getName();

    virtual Interface *interface() override { return nullptr; }
};

} // namespace GenICam

} // namespace Jgv
