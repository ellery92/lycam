#pragma once
#include "inode.h"
#include "iport.h"
#include "genicamxmlparser.h"

#include <boost/endian/conversion.hpp>
using namespace boost::endian;

#include <memory>
#include <string>

namespace Jgv {

namespace GenICam {

class Node : public INode
{
public:
    Node(std::string const &name, std::weak_ptr<GenICamXMLParser> xmlParser,
         std::weak_ptr<IPort::Interface> port);

    static std::shared_ptr<Node>
    MakeNode(std::string const name,
             std::weak_ptr<GenICamXMLParser> xmlParser,
             std::weak_ptr<IPort::Interface> port);

    virtual ~Node() = default;

    virtual std::string typeString() override final;
    virtual std::string toolTip() override final;
    virtual std::string description() override final;
    virtual std::string displayName() override final;
    virtual std::string featureName() override final;
    virtual std::string visibility() override final;

    virtual Interface *interface() = 0;

protected:
    std::string getChildNodeValue(std::string const &name);
    std::string getNodeAttribute(std::string const &attr);

    Poco::XML::Element *_node;
    std::weak_ptr<GenICamXMLParser> _xmlParser;
    std::weak_ptr<IPort::Interface> _port;
};

} // namespace GenICam

} // namespace Jgv
