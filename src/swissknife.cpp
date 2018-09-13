#include "swissknife.h"
#include "logger.h"
#include "swissknifehelper.h"
#include <Poco/DOM/Node.h>
#include <Poco/DOM/Element.h>

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
