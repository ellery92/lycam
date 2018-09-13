/*  -*- mode: c++ -*- */
#pragma once
#include <vector>
#include <string>

class GvDeviceImpl;

class GvDevice
{
public:
    typedef std::vector<GvDevice> List;

    GvDevice(const GvDevice &dev);
    GvDevice &operator=(const GvDevice &dev);
    ~GvDevice();

    std::string localIP();
    std::string deviceIP();
    std::string manufacturerName();
    std::string modelName();
    std::string version();
    std::string serialNumber();
    std::string userDefinedName();

    GvDeviceImpl& impl() { return *_pImpl; }

    static List list();

private:
    GvDevice(std::string const &locIP, std::string const &devIP);
    void swap(GvDevice &other);
    GvDeviceImpl* _pImpl;
};
