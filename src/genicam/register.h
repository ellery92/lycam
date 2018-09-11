#pragma once
#include "node.h"
#include "iregister.h"

namespace Jgv {

namespace GenICam {

class RegisterNode final : public Node, public Register::Interface
{
public:
    virtual ~RegisterNode();
    virtual void get(uint8_t *buffer, uint64_t length) override;
    virtual void set(uint8_t *buffer, uint64_t length) override;
    virtual uint64_t getAddress() override;
    virtual uint64_t getLength() override;
};

} // namespace GenICam

} // namespace Jgv
