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

#ifndef IENUMERATION_H
#define IENUMERATION_H

#include "iinterface.h"
#include <vector>

namespace Jgv {

namespace GenICam {

namespace Enumentry {
class Object;
}

namespace Enumeration {

class Interface : public GenICam::Interface
{
public:
    typedef std::vector<Enumentry::Object *> EntryList;

    virtual ~Interface() = default;

    virtual std::string getStringValue() = 0;
    virtual void setStringValue(const std::string &value) = 0;
    virtual uint64_t getIntValue() const = 0;
    virtual void setIntValue(uint64_t value) = 0;
    virtual EntryList  getEntries() const = 0;

    Type interfaceType() const override final {
        return Type::IEnumeration;
    }
    std::string interfaceTypeString() const override final {
        return "IEnumeration";
    }

}; // class Interface

} // namespace Enumeration

} // namespace GenICam

} // namespace Jgv

#endif // IENUMERATION_H
