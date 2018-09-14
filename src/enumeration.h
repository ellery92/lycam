#pragma once
#include "node.h"
#include "ienumeration.h"

namespace Jgv {

namespace GenICam {

class EnumerationNode : public Node, public Enumeration::Interface
{
public:
    EnumerationNode(std::string const &name, GenICamXMLParser::WeakPtr xmlParser,
                    IPort::Interface::WeakPtr port)
        : Node(name, xmlParser, port) {}

    virtual ~EnumerationNode() = default;
    virtual std::string getStringValue();
    virtual void setStringValue(const std::string &value);
    virtual uint64_t getIntValue();
    virtual void setIntValue(uint64_t value);
    virtual Enumeration::Interface::EntryList  getEntries() const;

    virtual Interface *interface() override {
        return this;
    }
};

} // namespace GenICam

} // namespace Jgv
