/*  -*- mode: c++ -*- */
#pragma once
#include "node.h"
#include "iinteger.h"
#include <memory>

namespace Jgv {

namespace GenICam {

class MaskedIntNode final : public Node, public Integer::Interface
{
public:
    MaskedIntNode(std::string const &name, GenICamXMLParser::WeakPtr xmlParser,
                IPort::Interface::WeakPtr port)
        : Node(name, xmlParser, port) {}

    virtual ~MaskedIntNode() = default;
    virtual uint64_t getValue() override;
    virtual void setValue(uint64_t value) override;
    virtual uint64_t getMin() override { return 0; };
    virtual uint64_t getMax() override { return 0; };
    virtual uint64_t getInc() override { return 0; };

    virtual Interface *interface() override { return this; }

private:
    void getLSBAndMSB(uint8_t &lsb, uint8_t &msb);
};

} // namespace GenICam

} // namespace Jgv
