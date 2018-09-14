#include "maskedintreg.h"
#include <boost/endian/conversion.hpp>
#include "logger.h"

using namespace Jgv::GenICam;
using namespace boost::endian;

namespace {
constexpr const uint64_t MASK[64] = {
    0x0000000000000001,0x0000000000000003,0x0000000000000007,0x000000000000000F,0x000000000000001F,0x000000000000003F,0x000000000000007F,0x00000000000000FF,
    0x00000000000001FF,0x00000000000003FF,0x00000000000007FF,0x0000000000000FFF,0x0000000000001FFF,0x0000000000003FFF,0x0000000000007FFF,0x000000000000FFFF,
    0x000000000001FFFF,0x000000000003FFFF,0x000000000007FFFF,0x00000000000FFFFF,0x00000000001FFFFF,0x00000000003FFFFF,0x00000000007FFFFF,0x0000000000FFFFFF,
    0x0000000001FFFFFF,0x0000000003FFFFFF,0x0000000007FFFFFF,0x000000000FFFFFFF,0x000000001FFFFFFF,0x000000003FFFFFFF,0x000000007FFFFFFF,0x00000000FFFFFFFF,
    0x00000001FFFFFFFF,0x00000003FFFFFFFF,0x00000007FFFFFFFF,0x0000000FFFFFFFFF,0x0000001FFFFFFFFF,0x0000003FFFFFFFFF,0x0000007FFFFFFFFF,0x000000FFFFFFFFFF,
    0x000001FFFFFFFFFF,0x000003FFFFFFFFFF,0x000007FFFFFFFFFF,0x00000FFFFFFFFFFF,0x00001FFFFFFFFFFF,0x00003FFFFFFFFFFF,0x00007FFFFFFFFFFF,0x0000FFFFFFFFFFFF,
    0x0001FFFFFFFFFFFF,0x0003FFFFFFFFFFFF,0x0007FFFFFFFFFFFF,0x000FFFFFFFFFFFFF,0x001FFFFFFFFFFFFF,0x003FFFFFFFFFFFFF,0x007FFFFFFFFFFFFF,0x00FFFFFFFFFFFFFF,
    0x01FFFFFFFFFFFFFF,0x03FFFFFFFFFFFFFF,0x07FFFFFFFFFFFFFF,0x0FFFFFFFFFFFFFFF,0x1FFFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFFF,0x7FFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF,
};
} // anonymous namespace

namespace Properties {
    constexpr const char * LSB = "LSB";
    constexpr const char * MSB = "MSB";
    constexpr const char * Bit = "Bit";
    constexpr const char * Endianess = "Endianess";
    constexpr const char * BigEndian = "BigEndian";
    constexpr const char * LittleEndian = "LittleEndian";
} // Properties

static uint64_t getFrom32LE(const uint8_t *registerValues, uint8_t msb, uint8_t lsb)
{
    const uint32_t val = little_to_native(*(uint32_t*)registerValues);
    // on décale du LSB puis applique le masque
    uint64_t res =  MASK[(msb-lsb)] & (val >> lsb);
    return res;
}

static uint64_t getFrom32BE(const uint8_t *registerValues, uint8_t msb, uint8_t lsb)
{
    // conversion BE -> LE
    const unsigned m = 31-msb;
    const unsigned l = 31-lsb;

    const uint32_t val = // big_to_native
        (*(uint32_t*)registerValues);
    // on décale du LSB puis applique le masque
    uint64_t res = MASK[m-l] & (val >> l);
    return res;
}

static uint64_t getFrom64LE(const uint8_t *registerValues, uint8_t msb, uint8_t lsb)
{
    const uint64_t val = little_to_native(*(uint64_t*)registerValues);
    // on décale du LSB puis applique le masque
    uint64_t res =  MASK[(msb-lsb)] & (val >> lsb);
    return res;
}

static uint64_t getFrom64BE(const uint8_t *registerValues, uint8_t msb, uint8_t lsb)
{
    // conversion BE -> LE
    const unsigned m = 63-msb;
    const unsigned l = 63-lsb;

    const uint64_t val = big_to_native(*(uint64_t*)registerValues);
    // on décale du LSB puis applique le masque
    uint64_t res = MASK[m-l] & (val >> l);
    return res;
}

static void setTo32LE(const uint8_t *registerValues, uint8_t msb, uint8_t lsb, uint32_t value)
{
    // calcul du masque
    const uint32_t mask = MASK[msb-lsb] << lsb;
    // extraction du registre
    uint32_t regVal = little_to_native(*(uint32_t*)registerValues);
    // mise à zéro des bits concernés
    regVal &= (~mask);
    // aligne la valeur du champs de bits
    uint32_t alignedValue = value << lsb;
    // applique au registre
    regVal |= alignedValue;
    // diffuse sur le tableau
    regVal = native_to_little(regVal);
    *(uint32_t *)registerValues = regVal;
}

static void setTo32BE(const uint8_t *registerValues, uint8_t msb, uint8_t lsb, uint32_t value)
{
    // conversion BE -> LE
    const unsigned m = 31-msb;
    const unsigned l = 31-lsb;

    // calcul du masque
    const uint32_t mask = MASK[m-l] << l;
    // extraction du registre
    uint32_t regVal = // big_to_native
        (*(uint32_t*)registerValues);
    // mise à zéro des bits concernés
    regVal &= (~mask);
    // aligne la valeur du champs de bits
    uint32_t alignedValue = value << l;
    // applique au registre
    regVal |= alignedValue;
    // diffuse sur le tableau
    // regVal = native_to_big(regVal);
    *(uint32_t*)registerValues = regVal;
}

static void setTo64LE(const uint8_t *registerValues, uint8_t msb, uint8_t lsb, uint32_t value)
{
    // calcul du masque
    const uint64_t mask = MASK[msb-lsb] << lsb;
    // extraction du registre
    uint64_t regVal = little_to_native(*(uint64_t*)registerValues);
    // mise à zéro des bits concernés
    regVal &= (~mask);
    // aligne la valeur du champs de bits
    uint64_t alignedValue = value << lsb;
    // applique au registre
    regVal |= alignedValue;
    // diffuse sur le tableau
    regVal = native_to_little(*(uint64_t*)regVal);
    *(uint64_t*)registerValues = regVal;
}

static void setTo64BE(const uint8_t *registerValues, uint8_t msb, uint8_t lsb, uint32_t value)
{
    // conversion BE -> LE
    const unsigned m = 63-msb;
    const unsigned l = 63-lsb;

    // calcul du masque
    const uint64_t mask = MASK[m-l] << l;
    // extraction du registre
    uint64_t regVal = big_to_native(*(uint64_t*)registerValues);
    // mise à zéro des bits concernés
    regVal &= (~mask);
    // aligne la valeur du champs de bits
    uint64_t alignedValue = value << l;
    // applique au registre
    regVal |= alignedValue;
    // diffuse sur le tableau
    regVal = native_to_big(regVal);
    *(uint64_t*)registerValues = regVal;
}

void MaskedIntNode::getLSBAndMSB(uint8_t &lsb, uint8_t &msb)
{
    std::string strbit = getChildNodeValue(Properties::Bit);
    if (!strbit.empty()) {
        lsb = msb = stoi(strbit, 0, 0);
    } else {
        std::string strlsb = getChildNodeValue(Properties::LSB);
        lsb = stoi(strlsb, 0, 0);
        std::string strmsb = getChildNodeValue(Properties::MSB);
        msb = stoi(strmsb, 0, 0);
    }
}

uint64_t MaskedIntNode::getValue()
{
    auto port = _port.lock();
    if (!port) {
        lyu_error("port is NULL");
        return 0;
    }

    uint64_t addr = address();
    uint64_t len = length();
    uint64_t value64 = 0;
    port->read((uint8_t *)&value64, addr, len);

    lyu_debug("read from " << std::hex << addr
              << std::dec << "(" << len << "): " << std::hex << value64);

    std::string endianess = getChildNodeValue(Properties::Endianess);
    uint8_t lsb, msb;
    getLSBAndMSB(lsb, msb);
    if (len == 4) {
        uint32_t value32 = value64;
        if (endianess == Properties::LittleEndian) {
            return getFrom32LE((uint8_t*)&value32, msb, lsb);
        } else {
            return getFrom32BE((uint8_t*)&value32, msb, lsb);
        }
    } else {
        if (endianess == Properties::LittleEndian) {
            return getFrom64LE((uint8_t*)&value64, msb, lsb);
        } else {
            return getFrom64BE((uint8_t*)&value64, msb, lsb);
        }
    }
    return 0;
}

void MaskedIntNode::setValue(uint64_t value)
{
    auto port = _port.lock();
    if (!port) {
        lyu_error("port is NULL");
        return;
    }

    uint64_t addr = address();
    uint64_t len = length();
    uint64_t value64 = 0;
    port->read((uint8_t *)&value64, addr, len);

    std::string endianess = getChildNodeValue(Properties::Endianess);
    uint8_t lsb, msb;
    getLSBAndMSB(lsb, msb);
    if (len == 4) {
        uint32_t value32 = value64;
        if (endianess == Properties::LittleEndian) {
            setTo32LE((uint8_t*)&value32, msb, lsb, (uint32_t)value);
        } else {
            setTo32BE((uint8_t*)&value32, msb, lsb, (uint32_t)value);
        }
        lyu_debug("write " << value32 << " to " << std::hex << addr << "(" << len << ")");
        port->write((uint8_t *)&value32, addr, len);
    } else {
        if (endianess == Properties::LittleEndian) {
            setTo64LE((uint8_t*)&value64, msb, lsb, value);
        } else {
            setTo64BE((uint8_t*)&value64, msb, lsb, value);
        }
        lyu_debug("write " << value64 << " to " << std::hex << addr << "(" << len << ")");
        port->write((uint8_t *)&value64, addr, len);
    }
}
