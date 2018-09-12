#include "swissknife.h"
#include <common/logger.h>
#include "swissknifehelper.h"

using namespace Jgv::GenICam;

double SwissKnifeNode::getValue()
{
    SwissKnifeHelper swissKnife;
    swissKnife.initFormula(_node, _xmlParser, _port);
    double value = swissKnife.getValueFloat();
    return value;
}

void SwissKnifeNode::setValue(double value)
{
    lyu_warning("SwissKnifeNode cannot set value");
}
