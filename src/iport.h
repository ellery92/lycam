/***************************************************************************
 *   Copyright (C) 2014-2017 by Cyril BALETAUD                                  *
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

#ifndef IPORT_H
#define IPORT_H

#include <cstdint>
#include <memory>

namespace Jgv {

namespace GenICam {

namespace IPort {

class Interface
{
public:
    typedef std::shared_ptr<Interface> Ptr;
    typedef std::weak_ptr<Interface> WeakPtr;

    virtual ~Interface() = default;

    virtual void read(uint8_t *pBuffer, uint64_t address, uint64_t length) = 0;
    virtual void write(uint8_t *pBuffer, uint64_t address, uint64_t length) = 0;
    virtual void writereg(uint8_t *pBuffer, uint64_t address, uint64_t length) = 0;
    virtual void readreg(uint8_t *pBuffer, uint64_t address, uint64_t length) = 0;
};

} // namespace IPort

} // namespace GenICam

} // namespace Jgv

#endif // IPORT_H
