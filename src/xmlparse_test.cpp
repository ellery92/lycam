#include <gvcp/gvdevice.h>
#include <gvcp/gvclient.h>
#include <genicam/genicamxmlparser.h>
#include <cassert>

int main()
{
    GvDevice::List devList = GvDevice::list();
    for (auto &dev : devList) {
        std::shared_ptr<GvcpClient> gvcpClient = std::make_shared<GvcpClient>();
        gvcpClient->controlDevice(dev.localIP(), dev.deviceIP());

        std::string filename = gvcpClient->xmlFilename();
        assert(!filename.empty());

        std::string xmlFile = gvcpClient->xmlFile();
        assert(!xmlFile.empty());
        assert(xmlFile.substr(0, 5) == "<?xml");

        Jgv::GenICam::GenICamXMLParser::Ptr xmlParser
            = Jgv::GenICam::GenICamXMLParser::create();
        xmlParser->parse(xmlFile);
        assert(xmlParser->getNodeByNameAttribute("RegisterDescription"));

        gvcpClient->releaseDevice();
    }

    return 0;
}
