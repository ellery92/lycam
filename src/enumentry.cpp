#include "enumentry.h"

using namespace Jgv::GenICam;

uint64_t EnumEntryNode::getValue()
{
    return getValueInt("Value");
}

std::string EnumEntryNode::getName()
{
    return featureName();
}
