/***************************************************************************
 *   Copyright (C) 2014-2017 by Cyril BALETAUD                             *
 *   cyril.baletaud@gmail.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "headerhelper.h"
#include "gvcp.h"

#include <boost/endian/conversion.hpp>

using namespace Jgv::Gvcp;
using namespace boost::endian;

HeaderCmdHelper::HeaderCmdHelper(uint16_t command, uint16_t length)
    : PacketHelper(static_cast<std::size_t>(sizeof(CMD_HEADER) + length)),
      header(reinterpret_cast<CMD_HEADER *const>(data))
{
    header->hardKey = GVCP_HARD_KEY;
    // par défaut, on veut l'ack
    setAcknowledge(true);
    header->command = native_to_big(command);
    header->length = native_to_big(length);
}

void HeaderCmdHelper::setAcknowledge(bool ack)
{
    (ack) ? header->flag|=0x01 : header->flag&=0x00;
}

bool HeaderCmdHelper::acknowledge() const
{
    return 1 == (header->flag & 0x01);
}

void HeaderCmdHelper::setReqId(uint16_t id)
{
    header->reqId = native_to_big(id);
}

uint16_t HeaderCmdHelper::reqId() const
{
    return big_to_native(header->reqId);
}

bool HeaderCmdHelper::isValid(const CMD_HEADER &header, uint16_t packetSize)
{
    if (packetSize < sizeof(ACK_HEADER)) {
        //qWarning("Ack header size too small: %d", packetSize);
        return false;
    }
    else if (header.flag != GVCP_HARD_KEY) {
        //qWarning("Bad cmd header flag: %x (not 0x42)", header.flag);
        return false;
    }
    else if (packetSize != (sizeof(ACK_HEADER) + length(header))) {
        //qWarning("Bad cmd header size: %d, expected: %lu", packetSize, sizeof(ACK_HEADER) + length(header));
        return false;
    }
    return true;
}

bool HeaderCmdHelper::acknowledgeIsSet(const CMD_HEADER &header)
{
    return (header.flag & 0x01) == 1;
}

uint16_t HeaderCmdHelper::command(const CMD_HEADER &header)
{
    return big_to_native(header.command);
}

uint16_t HeaderCmdHelper::length(const CMD_HEADER &header)
{
    return big_to_native(header.length);
}

uint16_t HeaderCmdHelper::reqId(const CMD_HEADER &header)
{
    return big_to_native(header.reqId);
}

HeaderAckHelper::HeaderAckHelper(uint16_t acknowledge, uint16_t length)
    : PacketHelper(sizeof(ACK_HEADER) + length),
      header(reinterpret_cast<ACK_HEADER * const>(data))
{
    header->status = native_to_big(static_cast<uint16_t>(Status::SUCCESS));
    header->acknowledge = native_to_big(acknowledge);
    header->length = native_to_big(length);
}

void HeaderAckHelper::changeStatus(uint16_t newStatus)
{
    header->status = native_to_big(newStatus);
}

void HeaderAckHelper::setAckID(uint16_t id)
{
    header->ackId = native_to_big(id);
}

bool HeaderAckHelper::isValid(const ACK_HEADER &header, uint16_t packetSize)
{
    if (packetSize < sizeof(ACK_HEADER)) {
        //qWarning("Ack header size too small: %d", packetSize);
        return false;
    }
    else if (packetSize != (sizeof(ACK_HEADER) + length(header))) {
        //qWarning("Bad ack size: %d, expected: %lu", packetSize, sizeof(ACK_HEADER) + length(header));
        return false;
    }
    return true;
}

uint16_t HeaderAckHelper::status(const ACK_HEADER &header)
{
    return big_to_native(header.status);
}

uint16_t HeaderAckHelper::acknowledge(const ACK_HEADER &header)
{
    return big_to_native(header.acknowledge);
}

uint16_t HeaderAckHelper::length(const ACK_HEADER &header)
{
    return big_to_native(header.length);
}

uint16_t HeaderAckHelper::ackId(const ACK_HEADER &header)
{
    return big_to_native(header.ackId);
}






