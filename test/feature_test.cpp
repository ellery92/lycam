#include "gvdevice.h"
#include "gvcpclient.h"

#include "nodemap.h"
#include "inode.h"
#include "iinteger.h"
#include "ifloat.h"
#include "iboolean.h"
#include "istring.h"
#include "ienumeration.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace Jgv;
using namespace Jgv::GenICam;

static void nodeTest(NodeMap::Ptr nodemap, std::string const &name)
{
    INode::Ptr node = nodemap->getNode(name);
    GenICam::Interface *iface = node->interface();
    switch (iface->interfaceType()) {
    case Type::IInteger: {
        Integer::Interface *intPtr = dynamic_cast<Integer::Interface*>(iface);
        std::cout << name << " " << intPtr->getValue();
        std::cout << " Min: " << intPtr->getMin();
        std::cout << " Max: " << intPtr->getMax() << std::endl;
        int oldValue = intPtr->getValue();
        int newValue = (intPtr->getMin() + intPtr->getMax()) / 2;
        intPtr->setValue(newValue);
        assert(intPtr->getValue() == newValue);
        intPtr->setValue(oldValue);
        assert(intPtr->getValue() == oldValue);
        break;
    }
    case Type::IFloat: {
        Float::Interface *fltPtr = dynamic_cast<Float::Interface*>(iface);

        std::cout << name << " " << fltPtr->getValue();
        std::cout << " Min: " << fltPtr->getMin();
        std::cout << " Max: " << fltPtr->getMax();

        double oldValue = fltPtr->getValue();
        double newValue = 3;
        fltPtr->setValue(3);
        std::cout << " " << fltPtr->getValue() << "=" << newValue;
        // assert(abs(fltPtr->getValue() - newValue) < 0.5);
        fltPtr->setValue(oldValue);
        // assert(abs(fltPtr->getValue() - oldValue) < 0.5);
        std::cout << " " << fltPtr->getValue() << "=" << oldValue;
        std::cout << std::endl;
        break;
    }
    case Type::IBoolean: {
        Boolean::Interface *blnPtr = dynamic_cast<Boolean::Interface*>(iface);
        if (blnPtr->getValue()) {
            std::cout << name << " enabled\n";
            blnPtr->setValue(false);
            assert(blnPtr->getValue() == false);
            blnPtr->setValue(true);
            assert(blnPtr->getValue() == true);
        } else {
            std::cout << name << " NOT enabled\n";
            blnPtr->setValue(true);
            assert(blnPtr->getValue() == true);
            blnPtr->setValue(false);
            assert(blnPtr->getValue() == false);
        }
        break;
    }
    case Type::IString: {
        String::Interface *strPtr = dynamic_cast<String::Interface*>(iface);
        std::cout << name << ":" << strPtr->getValue() << std::endl;
        break;
    }
    case Type::IEnumeration: {
        Enumeration::Interface *enmPtr = dynamic_cast<Enumeration::Interface*>(iface);
        std::cout << name << ": " << enmPtr->getStringValue() << ":" << enmPtr->getIntValue() << std::endl;
        Enumeration::Interface::EntryList entris = enmPtr->getEntries();
        std::string newName;
        for (auto &p : entris) {
            std::cout << p->getName() << ": " << p->getValue() << std::endl;
            if (p->getName() != enmPtr->getStringValue()) {
                newName = p->getName();
            }
        }

        if (!newName.empty()) {
            enmPtr->setStringValue(newName);
            std::cout << newName << " = " << enmPtr->getStringValue() << std::endl;
            // assert(enmPtr->getStringValue() == newName);
        }
        break;
    }
    }
}

int main()
{
    std::cout << "run feature test:\n";
    GvDevice::List devList = GvDevice::list();
    for (auto &dev : devList) {
        std::cout << dev.deviceIP() << std::endl;
        std::shared_ptr<GvcpClient> gvcpClient = std::make_shared<GvcpClient>();
        gvcpClient->controlDevice(dev.localIP(), dev.deviceIP());

        std::string xmlFile = gvcpClient->xmlFile();

        GenICamXMLParser::Ptr xmlParser = GenICamXMLParser::create();
        xmlParser->parse(xmlFile);

        NodeMap::Ptr nodeMap = NodeMap::create(xmlParser, gvcpClient);

        nodeTest(nodeMap, "Width");
        nodeTest(nodeMap, "Gain");
        nodeTest(nodeMap, "SpatialFilterEnable");
        nodeTest(nodeMap, "DeviceModelName");
        nodeTest(nodeMap, "DeviceSerialNumber");
        nodeTest(nodeMap, "AcquisitionMode");
        gvcpClient->releaseDevice();
    }

    return 0;
}
