#pragma once
#include "inode.h"
#include "iport.h"
#include "genicamxmlparser.h"

#include <vector>

namespace Jgv {

namespace GenICam {

class NodeMap
{
public:
    typedef std::shared_ptr<NodeMap> Ptr;
    static Ptr create(GenICamXMLParser::Ptr xmlParser, IPort::Interface::Ptr port);

    virtual ~NodeMap() = default;

    virtual std::vector<std::string> categoryNames() = 0;
    virtual std::vector<std::string> featureNames(std::string const &category) = 0;

    virtual INode::Ptr getNode(std::string const &name) = 0;

    virtual void setFeature(std::string name) = 0;
    virtual void setFeature(std::string name, std::string value) = 0;
    virtual void getFeature() = 0;
};

} // namespace GenICam

} // namespace Jgv
