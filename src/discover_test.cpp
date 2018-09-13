#include <gvcp/gvdevice.h>
#include <cassert>

int main()
{
    GvDevice::List devList = GvDevice::list();
    for (auto &dev : devList) {
        assert(!dev.localIP().empty());
        assert(!dev.deviceIP().empty());
        assert(!dev.manufacturerName().empty());
        assert(!dev.modelName().empty());
        assert(!dev.serialNumber().empty());
    }

    return 0;
}
