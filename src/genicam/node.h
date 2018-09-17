#pragma once
#include "inode.h"
#include "iport.h"
#include "genicamxmlparser.h"

#include <memory>
#include <string>

namespace Poco {
    namespace XML {
        class Element;
        class Node;
    }
}

namespace Jgv {

namespace GenICam {

class Node : public INode
{
public:
    typedef std::shared_ptr<Node> Ptr;

    static Ptr create(std::string const &name, GenICamXMLParser::WeakPtr xmlParser,
                      IPort::Interface::WeakPtr port);

    Node(std::string const &name, GenICamXMLParser::WeakPtr xmlParser,
         IPort::Interface::WeakPtr port);

    Node(Poco::XML::Element *node, GenICamXMLParser::WeakPtr xmlParser,
         IPort::Interface::WeakPtr port);

    virtual ~Node() = default;

    virtual std::string typeString() override final;
    virtual std::string toolTip() override final;
    virtual std::string description() override final;
    virtual std::string displayName() override final;
    virtual std::string featureName() override final;
    virtual std::string visibility() override final;

    virtual Interface *interface() override = 0;

protected:
    virtual uint64_t address() override final;
    virtual uint64_t length() override final;

    virtual uint64_t getValueInt(std::string const &name) override final;
    virtual double getValueFloat(std::string const &name) override final;

    std::string getChildNodeValue(std::string const &name);
    std::string getNodeAttribute(std::string const &attr);

protected:
    Node() = default;
    Poco::XML::Element *_node;
    GenICamXMLParser::WeakPtr _xmlParser;
    IPort::Interface::WeakPtr _port;
};

} // namespace GenICam

} // namespace Jgv
