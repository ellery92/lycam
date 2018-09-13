#pragma once
#include "node.h"
#include "ienumeration.h"

namespace Jgv {

namespace GenICam {

class EnumerationNode final : public Node, public Enumeration::Interface
{
public:
    virtual ~EnumerationNode();
    virtual std::string getStringValue() = 0;
    virtual void setStringValue(const std::string &value) = 0;
    virtual uint64_t getIntValue() const = 0;
    virtual void setIntValue(uint64_t value) = 0;
    virtual Enumeration::Interface::EntryList  getEntries() const = 0;
};

} // namespace GenICam

} // namespace Jgv
