#pragma once
#include "node.h"
#include <memory>

#include "iinterface.h"

namespace Jgv {

namespace GenICam {

class RegNode : public Node
{
public:
    typedef std::shared_ptr<RegNode> Ptr;

    RegNode(std::string const &name, std::weak_ptr<GenICamXMLParser> xmlParser,
        std::weak_ptr<IPort::Interface> port)
        : Node(name, xmlParser, port) {}
    virtual ~RegNode() = default;

    static Ptr create(std::string const &name,
                      std::weak_ptr<GenICamXMLParser> xmlParser,
                      std::weak_ptr<IPort::Interface> port);

    uint64_t getAddr();
    uint64_t getLength();

    uint64_t getValueInt(std::string const &name);
    double getValueFloat(std::string const &name);

    Type interfaceType();
    virtual Interface *interface() = 0;
};

} // namespace GenICam

} // namespace Jgv
