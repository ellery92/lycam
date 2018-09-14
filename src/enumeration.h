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
    virtual std::string getStringValue() override;
    virtual void setStringValue(const std::string &value) override;
    virtual uint64_t getIntValue() override;
    virtual void setIntValue(uint64_t value) override;
    virtual Enumeration::Interface::EntryList  getEntries() const override;

    virtual Interface *interface() override {
        return this;
    }
};

} // namespace GenICam

} // namespace Jgv
