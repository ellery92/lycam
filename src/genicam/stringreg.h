#pragma once
#include "node.h"
#include "istring.h"

namespace Jgv {

namespace GenICam {

class StringRegNode final : public Node, public String::Interface
{
public:
    virtual ~StringRegNode();
    virtual std::string getValue() override;
    virtual void setValue(const std::string &value) override;
    virtual uint64_t getMaxLenght() override;
};

} // namespace GenICam

} // namespace Jgv
