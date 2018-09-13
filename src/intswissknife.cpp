#include "intswissknife.h"
#include "logger.h"
#include "swissknifehelper.h"
#include <Poco/DOM/Node.h>
#include <Poco/DOM/Element.h>

using namespace Jgv::GenICam;

uint64_t IntSwissKnifeNode::getValue()
{
    SwissKnifeHelper swissKnife;
    swissKnife.initFormula(_node, _xmlParser, _port);
    uint64_t value = swissKnife.getValueInt();
    return value;
}

void IntSwissKnifeNode::setValue(uint64_t value)
{
    lyu_warning("IntSwissKnifeNode cannot set value");
}
