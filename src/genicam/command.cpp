#include "command.h"

using namespace Jgv::GenICam;

void CommandNode::execute()
{
    std::string pValue = getNodeValue("pValue");

}

bool CommandNode::isDone()
{
    return true;
}
