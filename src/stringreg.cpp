#include "stringreg.h"
#include "iinteger.h"
#include <vector>
#include <stack>

#include "logger.h"

using namespace Jgv::GenICam;

std::string StringRegNode::getValue()
{
    auto port = _port.lock();
    if (!port) {
        return 0;
    }

    uint64_t addr = address();
    uint64_t len = length();
    std::vector<char> res(len+1);
    char *data = res.data();
    port->read((uint8_t*)data, addr, len);

    char *value = new char[len+1];
    memset(value, 0, len+1);
    char *p = value;

    std::stack<char> st;
    for (uint64_t i = 0; i < len; i++) {
        st.push(data[i]);
        if (st.size() == 4) {
            while (!st.empty()) {
                *p++ = st.top();
                st.pop();
            }
        }
    }
    while (!st.empty()) {
        *p++ = st.top();
        st.pop();
    }

    std::string strvalue(value);
    delete[] value;

    lyu_debug("read from " << std::hex << addr
              << "(" << std::dec << len << "): " << strvalue);
    return strvalue;
}

void StringRegNode::setValue(std::string const &value)
{
    auto port = _port.lock();
    if (!port) {
        return;
    }

    uint64_t addr = address();
    uint64_t len = std::min(length(), value.length());

    lyu_debug("write " << value << " to " << std::hex << addr << "(" << len << ")");
    port->write((uint8_t *)value.data(), addr, len);
}

uint64_t StringRegNode::getMaxLength()
{
    return length();
}
