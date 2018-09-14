#include "gvcp.h"
#include "gvdevice.h"
#include "discoveryhelper.h"
#include <iostream>
#include <Poco/Net/NetworkInterface.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/RefCountedObject.h>
#include "logger.h"

using namespace Poco;
using namespace Poco::Net;

using namespace Jgv::Gvcp;

union Packet {
    char buffer[sizeof(DISCOVERY_ACK)];
    ACK_HEADER headerAck;
    DISCOVERY_ACK discoveryAck;
};

class GvDeviceImpl : public RefCountedObject
{
public:
    GvDeviceImpl(std::string const &locIP, std::string const &devIP)
        : _locIP(locIP), _devIP(devIP) {}

    std::string localIP() {
        return _locIP;
    }

    std::string deviceIP() {
        return _devIP;
    }

    uint16_t specVersionMajor() {
        return _data.specVersionMajor;
    }

    uint16_t specVersionMinor() {
        return _data.specVersionMinor;
    }

    uint32_t deviceMode() {
        return _data.deviceMode;
    }

    uint16_t deviceMACAddressHigh() {
        return _data.deviceMACAddressHigh;
    }

    uint32_t deviceMACAddressLow() {
        return _data.deviceMACAddressLow;
    }

    uint32_t ipConfigOptions() {
        return _data.ipConfigOptions;
    }

    uint32_t ipConfigCurrent() {
        return _data.ipConfigCurrent;
    }

    uint32_t currentIP() {
        return _data.currentIP;
    }

    uint32_t currentSubnetMask() {
        return _data.currentSubnetMask;
    }

    uint32_t defaultGateway() {
        return _data.defaultGateway;
    }

    const char *manufacturerName() {
        return _data.manufacturerName;
    }

    const char *modelName() {
        return _data.modelName;
    }

    const char *deviceVersion() {
        return _data.deviceVersion;
    }

    const char *manufacturerSpecificInformation() {
        return _data.manufacturerSpecificInformation;
    }

    const char *serialNumber() {
        return _data.serialNumber;
    }

    const char *userDefinedName() {
        return _data.userDefinedName;
    }

    void setData(DISCOVERY_ACK *data) {
        memcpy(&_data, data, sizeof(_data));
    }

private:
    std::string _locIP;
    std::string _devIP;

    DISCOVERY_ACK _data;
};

static bool isPacketComplete(Packet const &packet, int size)
{
    if (size < sizeof(ACK_HEADER)) {
        lyu_warning("header size too small");
        return false;
    }

    if (HeaderAckHelper::status(packet.headerAck) != enumType(Status::SUCCESS)) {
        lyu_warning("gvDevice::list(): status unsuccess");
        return false;
    }

    if (HeaderAckHelper::ackId(packet.headerAck) != 0xffff) {
        lyu_warning("discover ack id is wrong");
        return false;
    }

    if (HeaderAckHelper::acknowledge(packet.headerAck) == GVCP_ACK_DISCOVERY) {
        if ( (HeaderAckHelper::length(packet.headerAck) != DISCOVERY_ACK_LENGTH)
             || (size != sizeof(DISCOVERY_ACK)) ) {
            lyu_warning("Discover mal formatted packet");
            return false;
        }
    }
    return true;
}

static bool discoverDevice(std::string const &locIP,
                           std::string const &broadIP,
                           Packet &packet,
                           SocketAddress &peerAddr)
{
    DatagramSocket socket;
    socket.bind(SocketAddress(locIP, 0));
    socket.setBroadcast(true);
    socket.setBlocking(true);

    DiscoveryCmdHelper cmd(DISCOVERY_CMD_LENGTH);
    cmd.allowBroadcastAck(true);
    cmd.setReqId(0xffff);

    socket.sendTo(cmd.data, cmd.size, SocketAddress(broadIP, GVCP_PORT));

    Poco::Timespan span(500000 * 10);
    if (socket.poll(span, Socket::SELECT_READ)) {
        int size = socket.receiveFrom(packet.buffer, sizeof(packet.buffer), peerAddr);

        if (isPacketComplete(packet, size))
            return true;
    }

    return false;
}

GvDevice::List GvDevice::list()
{
    GvDevice::List devLst;
    NetworkInterface::List nlist = NetworkInterface::list();
    for (auto &iface : nlist) {
        if (iface.type() == NetworkInterface::NI_TYPE_ETHERNET_CSMACD
            && iface.address().family() == AddressFamily::IPv4) {
            std::string locIP = iface.address().toString();
            lyu_debug("locIP: " << locIP);
            std::string broadIP = "255.255.255.255";
            Packet packet;
            SocketAddress peerAddr;
            if (discoverDevice(locIP, broadIP, packet, peerAddr)) {
                GvDevice dev(locIP, peerAddr.host().toString());
                dev.impl().setData(&packet.discoveryAck);
                devLst.push_back(dev);
            }
        }
    }

    return devLst;
}

GvDevice::GvDevice(std::string const &locIP, std::string const &devIP)
    : _pImpl(new GvDeviceImpl(locIP, devIP)) {}

GvDevice::~GvDevice()
{
    _pImpl->release();
}

GvDevice::GvDevice(const GvDevice &dev)
    : _pImpl(dev._pImpl)
{
    _pImpl->duplicate();
}

GvDevice &GvDevice::operator=(const GvDevice &dev)
{
    GvDevice tmp(dev);
    swap(tmp);
    return *this;
}

void GvDevice::swap(GvDevice &other)
{
    std::swap(_pImpl, other._pImpl);
}

std::string GvDevice::localIP() {
    return _pImpl->localIP();
}

std::string GvDevice::deviceIP() {
    return _pImpl->deviceIP();
}

std::string GvDevice::manufacturerName() {
    return std::string(_pImpl->manufacturerName());
}

std::string GvDevice::modelName() {
    return std::string(_pImpl->modelName());
}

std::string GvDevice::version() {
    return std::string(_pImpl->deviceVersion());
}

std::string GvDevice::serialNumber() {
    return std::string(_pImpl->serialNumber());
}

std::string GvDevice::userDefinedName() {
    return std::string(_pImpl->userDefinedName());
}
