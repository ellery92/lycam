#include "gvcpclient.h"

#include "gvcp.h"
#include "headerhelper.h"
#include "readmemhelper.h"
#include "writememhelper.h"
#include "readreghelper.h"
#include "writereghelper.h"
#include "bootstrapregisters.h"
#include "discoveryhelper.h"

#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>

#include <Poco/Net/DatagramSocket.h>
#include <Poco/Zip/ZipArchive.h>
#include <Poco/Zip/ZipStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/RefCountedObject.h>

#include "logger.h"

using namespace Jgv::Gvcp;
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Zip;

union GvcpBuffer {
    char data[GVCP_PACKET_MAX_SIZE];
    CMD_HEADER headerCmd;
    ACK_HEADER headerAck;
    DISCOVERY_ACK discoveryAck;
    READREG_CMD readregCmd;
    READREG_ACK readregAck;
    WRITEREG_CMD writeregCmd;
    WRITEREG_ACK writeregAck;
    READMEM_CMD readmemCmd;
    READMEM_ACK readmemAck;
    WRITEMEM_CMD writememCmd;
};
static GvcpBuffer buffer;

#if 1
std::string statusToString(uint16_t status) {
    switch (status) {
    case enumType(Status::SUCCESS): return "Succes";
    case enumType(Status::PACKET_RESEND): return "Packet resend";
    case enumType(Status::NOT_IMPLEMENTED): return "Not implemented";
    case enumType(Status::INVALID_PARAMETER): return "Invalid parameter";
    case enumType(Status::INVALID_ADDRESS): return "Invalid address";
    case enumType(Status::WRITE_PROTECT): return "Write protect";
    case enumType(Status::BAD_ALIGNMENT): return "Bad alignement";
    case enumType(Status::ACCESS_DENIED): return "Acces denied";
    case enumType(Status::PACKET_UNAVAILABLE): return "Packet unavailable";
    case enumType(Status::DATA_OVERUN): return "Data overun";
    case enumType(Status::INVALID_HEADER): return "Invalid header";
    case enumType(Status::PACKET_NOT_YET_AVAILABLE): return "Packet not yet available";
    case enumType(Status::PACKET_AND_PREV_REMOVED_FROM_MEMORY): return "Packet and prev removed from memory";
    case enumType(Status::PACKET_REMOVED_FROM_MEMORY): return "Packet removed from memory";
    case enumType(Status::NO_REF_TIME): return "No ref time";
    case enumType(Status::PACKET_TEMPORARILY_UNAVAILABLE): return "packet temporarily unavailable";
    case enumType(Status::STATUS_OVERFLOW): return "Status overflow";
    case enumType(Status::ACTION_LATE): return "Action late";
    case enumType(Status::STATUS_ERROR): return "Error";
    default: return "Unknow error";
    }
}
#endif

enum class State {none, controller, monitor};
class GvcpClientImpl : public RefCountedObject
{
public:
    void releaseDevice();
    bool controlDevice(std::string const &controllerIP, std::string const &deviceIP);
    bool monitorDevice(std::string const &monitorIP, std::string const &deviceIP);

    std::vector<uint32_t> readRegisters(const std::vector<uint32_t> &addresses);
    bool writeRegisters(const AddrValPairList &values);

    const uint8_t *readMemory(uint32_t address, uint16_t count);
    bool writeMemory(uint32_t address, const uint8_t *data, uint32_t size);

    std::string xmlFile();
    std::string xmlFilename();

private:
    bool proceed(HeaderCmdHelper &cmd, uint16_t ackType, bool resetHeartbeat = true);
    void doHeartbeat();

private:
    DatagramSocket socket;

    uint16_t _id = 1;
    std::mutex _proceedMutex;

    std::string _controllerIP;
    std::string _deviceIP;
    uint32_t _heartbeatTimeout = 500;
    uint32_t _timestampDateTimeout = 10000;
    uint64_t _timestamp = 0;

    std::unique_ptr<std::thread> _timerPtr;
    std::unique_ptr<std::thread> _timestampPtr;
    volatile bool _heartbeatEnable = true;
    std::mutex _mutex;
    std::condition_variable _cvHB;

    State _state = State::none;

    int _cmdRetry = 3;
};

bool GvcpClientImpl::proceed(HeaderCmdHelper &cmd, uint16_t ackType, bool resetHeartbeat)
{
    // assure l'appel threadsafe
    std::unique_lock<std::mutex> lock(_proceedMutex);

    // relance le timer heartbeat
    if (resetHeartbeat) {
        _cvHB.notify_all();
    }

    const uint16_t currentId = _id;
    _id = (_id == 0xFFFF) ? 1 : _id + 1;
    cmd.setReqId(currentId);

    int retry = _cmdRetry;
    // tant que le cycle des retry n'est pas fini, on réemet la commande
    while (retry > 0) {
        --retry;

        int slen = socket.sendTo(cmd.data, cmd.size, SocketAddress(_deviceIP, GVCP_PORT));

        // l'envoi n'est pas passé
        if (slen == -1) {
            lyu_error("GvcpClient failed");
        }
        if (static_cast<std::size_t>(slen) != cmd.size) {
            lyu_error("GvcpClientPrivate::proceed: Oups! send len != command size");
        }

        // si la commande n'a pas besoin d'acquiescement, on quitte
        if (!cmd.acknowledge()) {
            return true;
        }

        // se met en attente d'un paquet (200 ms timeout)
        Poco::Timespan span(200000);
        if (!socket.poll(span, Socket::SELECT_READ))
            continue;

        SocketAddress peerAddr;
        int len = socket.receiveFrom(buffer.data, GVCP_PACKET_MAX_SIZE, peerAddr);
        if (len < 0) {
            // raison inconnue, on se remet en attente poll
            lyu_error("GvcpClient proceed");
        }
        else if (!HeaderAckHelper::isValid(buffer.headerAck, len)) {
            lyu_error("GvcpClientPrivate::proceed: RecvLen errror: " << len);
        }
        else if (peerAddr.host().toString() != _deviceIP ||
                 peerAddr.port() != GVCP_PORT) {
            lyu_error("GvcpClientPrivate::proceed: invalide peer");
        }
        else if (HeaderAckHelper::ackId(buffer.headerAck) != cmd.reqId()) {
            lyu_error("GvcpClientPrivate::proceed: Ack id != Cmd id:");
        }
        // controle que l'ack est du bon type
        else if (HeaderAckHelper::acknowledge(buffer.headerAck) != ackType) {
            // mauvais ack, on se remet  en attente poll
            lyu_error("GvcpClientPrivate::proceed: Bad Ack");
        }

        // le status n'est pas succes
        else if (HeaderAckHelper::status(buffer.headerAck)
                 != enumType(Status::SUCCESS)) {
            // la commande n'est pas passée, on quitte
            lyu_warning("ack status failed: "
                        << statusToString(HeaderAckHelper::status(buffer.headerAck)));
            return false;
        }

        // tout va bien, on sort
        else {
            return true;
        }
    }
    lyu_warning("GvcpClientPrivate::proceed: complete cycle failed");
    return false;
}

void GvcpClientImpl::doHeartbeat()
{
    ReadregCmdHelper cmd(sizeof(uint32_t));
    cmd.setAddresses({enumType(BootstrapAddress::HeartbeatTimeout)});

    while (_heartbeatEnable) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_cvHB.wait_for(lock, std::chrono::milliseconds(_heartbeatTimeout))
            == std::cv_status::timeout) {
            // obtient le valeur du timeout heartbeat
            if (proceed(cmd, GVCP_ACK_READREG)) {
                std::vector<uint32_t>regs = ReadregAckHelper::aswers(buffer.readregAck);
                if (!regs.empty()) {
                    // on diminue le timeout de 400 ms
                    _heartbeatTimeout = regs.at(0) - 400;
                }
            }
        }
    }
}

bool GvcpClientImpl::controlDevice(std::string const &controllerIP,
                                    std::string const &deviceIP)
{
    if (_state == State::controller
        && _controllerIP == controllerIP
        && _deviceIP == deviceIP) {
        return true;
    }

    if (_state != State::none) {
        releaseDevice();
    }

    _controllerIP = controllerIP;
    _deviceIP = deviceIP;
    _state = State::controller;

    // descripteur du socket UDP
    socket.bind(SocketAddress(_controllerIP, 0));
    socket.setBlocking(true);

    // obtient CCP, si lecture impossible on a un soucis de communication
    std::vector<uint32_t> regs;
    regs = readRegisters({enumType(BootstrapAddress::ControlChannelPrivilege)});

    if (regs.empty()) {
        lyu_error("GvcpClient::connect failed: can't read CCP (exclusive access ?)");
        releaseDevice();
        return false;
    }

    CCPPrivilege devicePriv = CCP::privilegeFromCCP(regs.at(0));
    // s'assure qu'on a le droit de se connecter
    if (devicePriv != CCPPrivilege::OpenAcces) {
        lyu_error("GvcpClient failed to control device, not in OpenAcces");
        releaseDevice();
        return false;
    }

    // demande le privilège control
    if (!writeRegisters({ADDRESS_VALUE{enumType(BootstrapAddress::ControlChannelPrivilege),
                                       CCP::CCPfromPrivilege(CCPPrivilege::ControlAcces)}}))
    {
        lyu_error("GvcpClient failed to control device, can't write privilege");
        releaseDevice();
        return false;
    }

    // obtient le valeur du timeout heartbeat
    regs = readRegisters({enumType(BootstrapAddress::HeartbeatTimeout)});
    if (!regs.empty()) {
        // on diminue le timeout de 400 ms
        _heartbeatTimeout = regs.at(0) - 400;

        lyu_info("GvcpClient activates heartbeat " << regs.at(0)
                 << " ms, running at " << _heartbeatTimeout);
    }

    // lance le heartbeat
    _timerPtr.reset(new std::thread(&GvcpClientImpl::doHeartbeat, this));

#ifdef _DEBUG
    std::string manufacturerName, modelName;
    const uint8_t *p = readMemory(enumType(BootstrapAddress::ManufacturerName), enumType(BootstrapBlockSize::ManufacturerName));
    if (p != nullptr) {
        manufacturerName = std::string(reinterpret_cast<const char *>(p));
    }
    p = readMemory(enumType(BootstrapAddress::ModelName), enumType(BootstrapBlockSize::ModelName));
    if (p != nullptr) {
        modelName = std::string(reinterpret_cast<const char *>(p));
    }

    lyu_debug("GvcpClient device is " << manufacturerName << " " << modelName << std::endl);
#endif
    return true;
}

bool GvcpClientImpl::monitorDevice(std::string const &monitorIP,
                                   std::string const &deviceIP)
{
    if (_state == State::monitor && monitorIP == _controllerIP && deviceIP == _deviceIP) {
        lyu_info("GvcpClient allready monitoring " << deviceIP);
        return true;
    }

    if (_state != State::monitor) {
        releaseDevice();
    }

    // descripteur du socket UDP
    socket.bind(SocketAddress(_controllerIP, 0));
    socket.setBlocking(true);

    _state = State::monitor;
    _controllerIP = monitorIP;
    _deviceIP = deviceIP;

    // on a un socket en écoute on essaie de lire CCP
    std::vector<uint32_t> regs = readRegisters({enumType(BootstrapAddress::ControlChannelPrivilege)});
    if (regs.empty()) {
        lyu_error("GvcpClient can't read CCP (exclusive access ?)");
        releaseDevice();
        return false;
    }

    lyu_info("GvcpClient monitors " << deviceIP);

    return true;
}

void GvcpClientImpl::releaseDevice()
{
    if (_state == State::none) {
        return;
    }

    // si le heartbeat tourne, on le ferme
    if (_timerPtr && _timerPtr->joinable()) {
        // coupe le heartbeat
        _heartbeatEnable = false;
        _cvHB.notify_all();
        _timerPtr->join();
    }

    // si on est contrôleur
    if (_state == State::controller) {
        // on coupe le stream
        if (!writeRegisters({ADDRESS_VALUE{enumType(BootstrapAddress::ControlChannelPrivilege), 0x00000000}})) {
            lyu_warning("GvcpClient fails to clear Control Channel Privilege");
        }
    }

    // ferme le socket
    socket.close();
    _controllerIP.clear();
    _deviceIP.clear();
    _state = State::none;
}

std::vector<uint32_t> GvcpClientImpl::readRegisters(const std::vector<uint32_t> &addresses)
{
    if (_state == State::none) {
        lyu_warning("GvcpClient failed to read register, no device configured");
        return std::vector<uint32_t>();
    }

    // forge la commande
    ReadregCmdHelper cmd(addresses.size() * sizeof(uint32_t));
    cmd.setAddresses(addresses);

    std::vector<uint32_t> result;
    if (proceed(cmd, GVCP_ACK_READREG)) {
        result = ReadregAckHelper::aswers(buffer.readregAck);
    }
    return result;
}

bool GvcpClientImpl::writeRegisters(const AddrValPairList &values)
{
    if (_state != State::controller) {
        lyu_warning("GvcpClient failed to write register, we are not controller");
        return false;
    }


    // forge la commande
    WriteregCmdHelper cmd(values.size() * 2 * sizeof(uint32_t));
    cmd.setRegsValList(values);

    return proceed(cmd, GVCP_ACK_WRITEREG);
}

const uint8_t *GvcpClientImpl::readMemory(uint32_t address, uint16_t count)
{
    if (_state == State::none) {
        lyu_warning("GvcpClient failed to read memory, no device configured");
        return nullptr;
    }

    // forge la commande
    ReadmemCmdHelper cmd(READMEM_CMD_LENGTH);
    cmd.setAddress(address);
    cmd.setDataCount(count);

    if (proceed(cmd, GVCP_ACK_READMEM)) {
        return ReadmemAckHelper::dataPtr(buffer.readmemAck);
    }
    return nullptr;
}

bool GvcpClientImpl::writeMemory(uint32_t address, const uint8_t *data, uint32_t size)
{
    if (_state != State::controller) {
        lyu_warning("GvcpClient failed to write memory, we are not controller");
        return false;
    }

    // forge la commande
    WritememCmdHelper cmd(sizeof(uint32_t) + size);
    cmd.setAddress(address);
    cmd.setCmdData(data, size);
    bool ret = proceed(cmd, GVCP_ACK_WRITEMEM);
    return ret;
}

std::string GvcpClientImpl::xmlFile()
{
    // on récupère la l'URL
    std::string url {reinterpret_cast<const char *>(readMemory(enumType(BootstrapAddress::FirstURL), enumType(BootstrapBlockSize::FirstURL)))};

    lyu_debug("xml url: " << url);

    // si ne commence pas par Local: on quitte
    if (url.compare(0, 6, "Local:") != 0) {
        return std::string();
    }

    // cherche les occurences du séparateur de champs
    url = url.substr(6);
    std::size_t from = 0;
    auto to = url.find_first_of(';');
    std::vector<std::string> split;
    while (to != std::string::npos) {
        split.emplace_back(url.substr(from, to - from));
        from = to + 1;
        to = url.find_first_of(';', to + 1);
    }
    split.emplace_back(url.substr(from));

    // si pas 3 champs, on quitte
    if (split.size() != 3) {
        lyu_error("xml url format is wrong");
        return std::string();
    }

    std::string filename = split[0];
    auto dotpos = filename.find_last_of('.');
    if (dotpos == std::string::npos) {
        lyu_error("xml file has no extenstion");
        return std::string();
    }

    std::string ext = filename.substr(dotpos+1);
    if (ext != "xml" && ext != "zip") {
        lyu_error("xml file extension " << ext << " not supported");
        return std::string();
    }

    std::size_t pos {0};
    auto address = std::stoul(split[1], &pos, 16);
    const auto size = std::stoul(split[2], &pos, 16);
    const auto end = address + size;

    lyu_debug(filename << " " << ext << " " << address << " " << size);

    // on construit un tampon de la taille du fichier
    std::vector<char> file (static_cast<std::size_t>(size));
    char *dest = file.data();
    while ((address+READMEM_ACK_PAYLOAD_MAX_SIZE) < end) {
        std::memcpy(dest, readMemory(static_cast<uint32_t>(address), READMEM_ACK_PAYLOAD_MAX_SIZE), READMEM_ACK_PAYLOAD_MAX_SIZE);
        address += READMEM_ACK_PAYLOAD_MAX_SIZE;
        dest += READMEM_ACK_PAYLOAD_MAX_SIZE;
    }
    // le dernier segment
    const auto lastSize =  BootstrapRegisters::align(size % READMEM_ACK_PAYLOAD_MAX_SIZE);
    std::memcpy(dest, readMemory(static_cast<uint32_t>(address), static_cast<uint16_t>(lastSize)), static_cast<std::size_t>(lastSize));

    std::string xmlfile;
    if (ext == "zip") {
        std::stringstream inp(std::ios::binary | std::ios_base::in | std::ios_base::out);
        inp.write(file.data(), size);
        ZipArchive arch(inp);
        std::string xmlfilename = filename.replace(dotpos+1, 3, "xml");
        ZipArchive::FileHeaders::const_iterator it = arch.findHeader(xmlfilename);
        if (it == arch.headerEnd()) {
            lyu_error("cannot find xml file " << xmlfilename << " in" << filename);
            return std::string();
        }
        ZipInputStream zipin(inp, it->second);
        std::ostringstream out(std::ios::binary);
        StreamCopier::copyStream(zipin, out);
        xmlfile = out.str();
    } else {
        xmlfile = file.data();
    }

    return xmlfile;
}

std::string GvcpClientImpl::xmlFilename()
{
    // on récupère l'URL
    std::string url {reinterpret_cast<const char *>(readMemory(enumType(BootstrapAddress::FirstURL), enumType(BootstrapBlockSize::FirstURL)))};

    // si ne commence pas par Local: on quitte
    if (url.compare(0, 6, "Local:") != 0) {
        return std::string();
    }

    // cherche les occurences du séparateur de champs
    const auto to = url.find_first_of(';');
    return to!=std::string::npos?url.substr(6, to - 6):std::string();
}


// intreface
GvcpClient::GvcpClient()
    : _pImpl(new GvcpClientImpl) {}

GvcpClient::~GvcpClient()
{
    releaseDevice();
    _pImpl->release();
}

GvcpClient::GvcpClient(const GvcpClient &dev)
    : _pImpl(dev._pImpl)
{
    _pImpl->duplicate();
}

GvcpClient &GvcpClient::operator=(const GvcpClient &dev)
{
    GvcpClient tmp(dev);
    swap(tmp);
    return *this;
}

void GvcpClient::swap(GvcpClient &other)
{
    std::swap(_pImpl, other._pImpl);
}

bool GvcpClient::controlDevice(std::string const &controllerIP, std::string const &deviceIP)
{
    return _pImpl->controlDevice(controllerIP, deviceIP);
}

bool GvcpClient::monitorDevice(std::string const &monitorIP, std::string const &deviceIP)
{
    return _pImpl->monitorDevice(monitorIP, deviceIP);
}

void GvcpClient::releaseDevice()
{
    _pImpl->releaseDevice();
}

std::vector<uint32_t> GvcpClient::readRegisters(const std::vector<uint32_t> &addresses)
{
    return _pImpl->readRegisters(addresses);
}

bool GvcpClient::writeRegisters(const AddrValPairList &values)
{
    return _pImpl->writeRegisters(values);
}

const uint8_t *GvcpClient::readMemory(uint32_t address, uint16_t count)
{
    return _pImpl->readMemory(address, count);
}

bool GvcpClient::writeMemory(uint32_t address, const uint8_t *data, uint32_t size)
{
    return _pImpl->writeMemory(address, data, size);
}

std::string GvcpClient::xmlFile()
{
    return _pImpl->xmlFile();
}

std::string GvcpClient::xmlFilename()
{
    return _pImpl->xmlFilename();
}

void GvcpClient::read(uint8_t *pBuffer, uint64_t address, uint64_t length)
{
    readreg(pBuffer, address, length);
#if 0
    const uint8_t *memory = readMemory(static_cast<uint32_t>(address),
                                       static_cast<uint16_t>(length));
    if (memory == nullptr) {
        lyu_error("GevIport failed to read address " << std::hex << address
                  << " of length " << length);
        return;
    }
    std::memcpy(pBuffer, memory, static_cast<std::size_t>(length));
#endif
}

void GvcpClient::readreg(uint8_t *pBuffer, uint64_t address, uint64_t length)
{
    std::vector<uint32_t> addrs;
    for (uint64_t i = 0; i < length; i+=4) {
        addrs.push_back(static_cast<uint32_t>(address));
        address += 4;
    }
    std::vector<uint32_t> res = readRegisters(addrs);
    uint8_t *p = pBuffer;
    for (size_t i = 0; i < res.size(); i++) {
        memcpy(p, (char*)&res[i], 4);
        p += 4;
    }
}

void GvcpClient::writereg(uint8_t *pBuffer, uint64_t address, uint64_t length)
{
    AddrValPairList list;
    uint32_t addr = static_cast<uint32_t>(address);
    uint8_t *pval = pBuffer;
    for (uint64_t i = 0; i < length; i+=4) {
        uint32_t val = *(uint32_t*)pval;
        list.push_back(ADDRESS_VALUE{addr, val});
        pval += 4;
        addr += 4;
    }

    if (!writeRegisters(list)) {
        lyu_error("GevIport failed to write address " << std::hex << address
                  << " of length " << length);
    }
}

void GvcpClient::write(uint8_t *pBuffer, uint64_t address, uint64_t length)
{
    writereg(pBuffer, address, length);
#if 0
    if (!writeMemory(address, pBuffer, length)) {
        lyu_error("GevIport failed to write address " << std::hex << address
                  << " of length " << length);
    }
#endif
}
