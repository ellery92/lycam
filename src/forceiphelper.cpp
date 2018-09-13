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

#include "forceiphelper.h"

#include <boost/endian/conversion.hpp>

using namespace Jgv::Gvcp;
using namespace boost::endian;

ForceipCmdHelper::ForceipCmdHelper(uint16_t length)
    : HeaderCmdHelper(GVCP_CMD_FORCEIP, length),
      cmd(reinterpret_cast<FORCEIP_CMD * const>(data))
{}

void ForceipCmdHelper::setMacHigh(uint16_t mac)
{
    cmd->MACAddressHigh = native_to_big(mac);
}

void ForceipCmdHelper::setMacLow(uint32_t mac)
{
    cmd->MACAddressLow = native_to_big(mac);
}

void ForceipCmdHelper::setStaticIP(uint32_t ip)
{
    cmd->staticIP = native_to_big(ip);
}

void ForceipCmdHelper::setStaticNetmask(uint32_t netmask)
{
    cmd->staticSubnetMask = native_to_big(netmask);
}

void ForceipCmdHelper::setStaticDefaultGateway(uint32_t gateway)
{
    cmd->staticDefaultGateway = native_to_big(gateway);
}

uint16_t ForceipCmdHelper::macHigh(const FORCEIP_CMD &cmd)
{
    return big_to_native(cmd.MACAddressHigh);
}

uint32_t ForceipCmdHelper::macLow(const FORCEIP_CMD &cmd)
{
    return big_to_native(cmd.MACAddressLow);
}

uint32_t ForceipCmdHelper::staticIP(const FORCEIP_CMD &cmd)
{
    return big_to_native(cmd.staticIP);
}

uint32_t ForceipCmdHelper::staticNetmask(const FORCEIP_CMD &cmd)
{
    return big_to_native(cmd.staticSubnetMask);
}

uint32_t ForceipCmdHelper::staticDefaultGateway(const FORCEIP_CMD &cmd)
{
    return big_to_native(cmd.staticDefaultGateway);
}

ForceipAckHelper::ForceipAckHelper(uint16_t length)
    : HeaderAckHelper(GVCP_CMD_FORCEIP, length),
      ack(reinterpret_cast<FORCEIP_ACK * const>(data))
{
    (void)ack;
}

