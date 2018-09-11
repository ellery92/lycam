#include <iostream>
#include <gvcp/gvdevice.h>
#include <gvcp/gvcpclient.h>
#include <genicam/nodemap.h>
#include <genicam/command.h>
#include <genicam/integer.h>
#include <genicam/floatjgv.h>

#include <fstream>
#include <sstream>

void testXML()
{
    std::fstream fs("XCG-CSeries_v_1_1_0.xml");
    std::stringstream ss;
    ss << fs.rdbuf();

    std::shared_ptr<Jgv::GenICam::GenICamXMLParser> xmlParser
        = std::make_shared<Jgv::GenICam::GenICamXMLParser>();
    xmlParser->parse(ss.str());

    std::shared_ptr<Jgv::GenICam::NodeMap> nodeMap =
        std::make_shared<Jgv::GenICam::NodeMap>(xmlParser,
             std::shared_ptr<Jgv::GenICam::IPort::Interface>(0));

    std::vector<std::string> categories = nodeMap->categoryNames();
    for (auto &p : categories) {
        std::cout << "Category: " << p << std::endl;
        std::vector<std::string> features = nodeMap->featureNames(p);
        for (auto &f : features) {
            std::cout << "\t" << f << std::endl;
        }
    }

    std::shared_ptr<Jgv::GenICam::INode> node = nodeMap->getNode("GigEVision");
    std::cout << node->typeString() << std::endl;
    std::cout << node->toolTip() << std::endl;
    std::cout << node->description() << std::endl;
    std::cout << node->displayName() << std::endl;
    std::cout << node->visibility() << std::endl;

    Jgv::GenICam::Interface *iface = node->interface();
    std::cout << iface->interfaceTypeString() << std::endl;

    node = nodeMap->getNode("Width");
    std::cout << node->typeString() << std::endl;
    std::cout << node->toolTip() << std::endl;
    std::cout << node->description() << std::endl;
    std::cout << node->displayName() << std::endl;
    std::cout << node->visibility() << std::endl;

    iface = node->interface();
    std::cout << iface->interfaceTypeString() << std::endl;
}

int main()
{
    GvDevice::List devList = GvDevice::list();
    std::cout << "Total GigEDevice: " << devList.size() << std::endl;
    for (auto &dev : devList) {
        std::cout << "local ip: " << dev.localIP() << std::endl;
        std::cout << "device ip: " << dev.deviceIP() << std::endl;
        std::cout << "manufacturername: " << dev.manufacturerName() << std::endl;
        std::cout << "model name: " << dev.modelName() << std::endl;
        std::cout << "serial number: " << dev.serialNumber() << std::endl;
        std::cout << "user defined name: " << dev.userDefinedName() << std::endl;

        std::shared_ptr<GvcpClient> gvcpClient = std::make_shared<GvcpClient>();
        gvcpClient->controlDevice(dev.localIP(), dev.deviceIP());

        std::string filename = gvcpClient->xmlFilename();
        std::cout << "xml file name: " << filename << std::endl;
        // std::cout << "xml content: " << gvcpClient.xmlFile();

        std::string xmlFile = gvcpClient->xmlFile();
        std::cout << xmlFile.substr(0, 150) << std::endl;
        if (xmlFile.empty()) {
            std::cout << "xmlFile is empty\n";
            return 0;
        }

        std::shared_ptr<Jgv::GenICam::GenICamXMLParser> xmlParser
            = std::make_shared<Jgv::GenICam::GenICamXMLParser>();
        xmlParser->parse(xmlFile);

        std::shared_ptr<Jgv::GenICam::NodeMap> nodeMap =
            std::make_shared<Jgv::GenICam::NodeMap>(xmlParser, gvcpClient);

        std::shared_ptr<Jgv::GenICam::INode> node = nodeMap->getNode("Width");
        poco_assert(node != NULL);
        Jgv::GenICam::Interface *iface = node->interface();
        poco_assert(iface->interfaceType() == Jgv::GenICam::Type::IInteger);
        Jgv::GenICam::Integer::Interface *intPtr =
            dynamic_cast<Jgv::GenICam::Integer::Interface*>(iface);
        std::cout << "Width: " << intPtr->getValue() << std::endl;
        std::cout << "Min: " << intPtr->getMin() << std::endl;
        std::cout << "Max: " << intPtr->getMax() << std::endl;
        std::cout << "Inc: " << intPtr->getInc() << std::endl;

        intPtr->setValue(1260);
        int width = intPtr->getValue();
        poco_assert(width == 1260);
        intPtr->setValue(1280);

        node = nodeMap->getNode("Gain");
        poco_assert(node != NULL);
        iface = node->interface();
        poco_assert(iface->interfaceType() == Jgv::GenICam::Type::IFloat);
        Jgv::GenICam::Float::Interface *fltPtr =
            dynamic_cast<Jgv::GenICam::Float::Interface*>(iface);
        std::cout << "Gain: " << fltPtr->getValue() << std::endl;

        gvcpClient->releaseDevice();
    }

    return 0;
}
