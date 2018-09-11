#pragma once
#include "genicamxmlparser.h"
#include "inode.h"
#include "iport.h"

namespace Jgv {

namespace GenICam {

class NodeMap
{
public:
    NodeMap(std::shared_ptr<GenICamXMLParser> xmlParser,
            std::shared_ptr<IPort::Interface> port);

    virtual ~NodeMap() = default;

    std::vector<std::string> categoryNames();
    std::vector<std::string> featureNames(std::string const &category);

    std::shared_ptr<INode> getNode(std::string const &name);

    void setFeature(std::string name);
    void setFeature(std::string name, std::string value);
    void getFeature();

private:
    std::shared_ptr<GenICamXMLParser> _xmlParser;
    std::shared_ptr<IPort::Interface> _port;
};

} // namespace GenICam

} // namespace Jgv
