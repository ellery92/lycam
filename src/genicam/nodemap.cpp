#include "nodemap.h"
#include "category.h"
#include "command.h"
#include "integer.h"
#include "floatjgv.h"

#include <common/logger.h>

using namespace Jgv::GenICam;
using namespace Poco;

NodeMap::NodeMap(std::shared_ptr<GenICamXMLParser> xmlParser,
                 std::shared_ptr<IPort::Interface> port)
    : _xmlParser(xmlParser), _port(port) {}

std::shared_ptr<INode> NodeMap::getNode(std::string const &name)
{
    Poco::XML::Node *node = _xmlParser->getNodeByNameAttribute(name);
    if (!node) {
        lyu_warning("no such feature: " << name);
        return NULL;
    }
    std::string type = node->nodeName();
    if (type == "Category") {
        return std::make_shared<CategoryNode>(name, _xmlParser, _port);
    } else if (type == "Command") {
        return std::make_shared<CommandNode>(name, _xmlParser, _port);
    } else if (type == "Integer") {
        return std::make_shared<IntegerNode>(name, _xmlParser, _port);
    } else if (type == "Float") {
        return std::make_shared<FloatNode>(name, _xmlParser, _port);
    }
    return NULL;
}

std::vector<std::string> NodeMap::categoryNames()
{
    return featureNames("Root");
}

std::vector<std::string> NodeMap::featureNames(std::string const &category)
{
    std::vector<std::string> res;
    Poco::XML::Node *node = _xmlParser->getNodeByNameAttribute(category);
    if (!node) {
        lyu_error("No such Node: " << category);
        return res;
    }

    Poco::XML::Node *p = node->firstChild();
    while (p) {
        if (p->nodeName() == "pFeature") {
            res.push_back(p->firstChild()->nodeValue());
        }
        p = p->nextSibling();
    }
    return res;
}
