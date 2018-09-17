#include "nodemap.h"
#include <Poco/DOM/Node.h>
#include <Poco/DOM/Element.h>
#include <util/logger.h>
#include "node.h"

using namespace Jgv::GenICam;
using namespace Poco;

class NodeMapImpl final : public NodeMap
{
public:
    NodeMapImpl(GenICamXMLParser::Ptr xmlParser, IPort::Interface::Ptr port);
    virtual ~NodeMapImpl() = default;

    virtual std::vector<std::string> categoryNames() override;
    virtual std::vector<std::string> featureNames(std::string const &category) override;

    virtual INode::Ptr getNode(std::string const &name) override;

    virtual void setFeature(std::string name) override {};
    virtual void setFeature(std::string name, std::string value) override {};
    virtual void getFeature() override {};

private:
    GenICamXMLParser::Ptr _xmlParser;
    IPort::Interface::Ptr _port;
};

NodeMapImpl::NodeMapImpl(GenICamXMLParser::Ptr xmlParser,
                 IPort::Interface::Ptr port)
    : _xmlParser(xmlParser), _port(port) {}

INode::Ptr NodeMapImpl::getNode(std::string const &name)
{
    return Node::create(name, _xmlParser, _port);
}

std::vector<std::string> NodeMapImpl::categoryNames()
{
    return featureNames("Root");
}

std::vector<std::string> NodeMapImpl::featureNames(std::string const &category)
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

NodeMap::Ptr NodeMap::create(GenICamXMLParser::Ptr xmlParser, IPort::Interface::Ptr port)
{
    return std::make_shared<NodeMapImpl>(xmlParser, port);
}
