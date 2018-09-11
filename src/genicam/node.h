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
    Node(std::string const &name,
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
    Poco::XML::Element *element();
    Poco::XML::Element *element(std::string const &name);

    std::string getNodeValue(Poco::XML::Node *node, const std::string &name);
    std::string getNodeValue(std::string const &name);
    std::string getNodeAttribute(std::string const &attr);

    void getAddrLength(Poco::XML::Node *node, uint64_t &addr,
                       uint64_t &length, std::string &endianess);

    template<typename T>
    T toNative(T value, std::string const &endianess);

    std::string _name;
    std::weak_ptr<GenICamXMLParser> _xmlParser;
    std::weak_ptr<IPort::Interface> _port;
};

template<typename T>
T Node::toNative(T value, std::string const &endianess)
{
    T res;
    if (endianess == "BigEndian") {
        res = big_to_native(value);
    } else {
        res = little_to_native(value);
    }
    return res;
}

} // namespace GenICam

} // namespace Jgv
