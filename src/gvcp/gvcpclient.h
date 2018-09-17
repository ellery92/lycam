#pragma once

#include <string>
#include <vector>
#include <genicam/iport.h>

namespace Jgv {
    namespace Gvcp {
        struct ADDRESS_VALUE;
        using AddrValPairList = std::vector<ADDRESS_VALUE>;
    }
}

class GvcpClientImpl;

class GvcpClient : public Jgv::GenICam::IPort::Interface
{
public:
    GvcpClient();
    virtual ~GvcpClient();
    GvcpClient(const GvcpClient &dev);
    GvcpClient &operator=(const GvcpClient &dev);

    bool controlDevice(std::string const &controllerIP, std::string const &deviceIP);
    bool monitorDevice(std::string const &monitorIP, std::string const &deviceIP);
    void releaseDevice();

    std::vector<uint32_t> readRegisters(const std::vector<uint32_t> &addresses);
    bool writeRegisters(const Jgv::Gvcp::AddrValPairList &values);

    const uint8_t *readMemory(uint32_t address, uint16_t count);
    bool writeMemory(uint32_t address, const uint8_t *data, uint32_t size);

    std::string xmlFile();
    std::string xmlFilename();

    // IPort
    void read(uint8_t *pBuffer, uint64_t address, uint64_t length) override final;
    void write(uint8_t *pBuffer, uint64_t address, uint64_t length) override final;
    void writereg(uint8_t *pBuffer, uint64_t address, uint64_t length) override final;
    void readreg(uint8_t *pBuffer, uint64_t address, uint64_t length) override final;

private:
    GvcpClientImpl *_pImpl;
    void swap(GvcpClient &other);
};
