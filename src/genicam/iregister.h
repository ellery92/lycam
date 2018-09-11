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

#ifndef IREGISTER_H
#define IREGISTER_H

#include "iinterface.h"

namespace Jgv {

namespace GenICam {

namespace Register {

class Interface : public GenICam::Interface
{
public:
    virtual ~Interface() = default;

    virtual void get(uint8_t *buffer, uint64_t length) = 0;
    virtual void set(uint8_t *buffer, uint64_t length) = 0;
    virtual uint64_t getAddress() = 0;
    virtual uint64_t getLength() = 0;

    Type interfaceType() const final {
        return Type::IRegister;
    }
    std::string interfaceTypeString() const final {
        return "IRegister";
    }
}; // class Interface

} // namespace Register

} // namespace GenICam

} // namespace Jgv

#endif // IREGISTER_H
