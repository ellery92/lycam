#pragma once
#include "node.h"
#include "ienumeration.h"

namespace Jgv {

namespace GenICam {

class EnumerationNode : public Node, public Enumeration::Interface
{
public:
    virtual ~EnumerationNode();
    virtual std::string getStringValue();
    virtual void setStringValue(const std::string &value);
    virtual uint64_t getIntValue();
    virtual void setIntValue(uint64_t value);
    virtual Enumeration::Interface::EntryList  getEntries() const;
};

} // namespace GenICam

} // namespace Jgv
