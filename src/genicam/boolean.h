#pragma once
#include "node.h"
#include "iboolean.h"

namespace Jgv {

namespace GenICam {

class BooleanNode final : public Node, public Boolean::Interface
{
public:
    bool getValue() override;
    void setValue(bool value) override;
};

} // namespace GenICam

} // namespace Jgv
