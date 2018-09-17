#include "enumentry.h"
#include <util/logger.h>
#include <Poco/DOM/Element.h>

using namespace Jgv::GenICam;

uint64_t EnumEntryNode::getValue()
{
    return getValueInt("Value");
}

std::string EnumEntryNode::getName()
{
    return featureName();
}
