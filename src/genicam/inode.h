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

#ifndef INODE_H
#define INODE_H

#include <string>
#include <memory>

namespace Jgv {

namespace GenICam {

class Interface;

class INode
{
public:
    typedef std::shared_ptr<INode> Ptr;

    virtual ~INode() = default;

    virtual std::string typeString() = 0;
    virtual std::string toolTip() = 0;
    virtual std::string description() = 0;
    virtual std::string displayName() = 0;
    virtual std::string featureName() = 0;
    virtual std::string visibility() = 0;
    virtual Interface *interface() = 0;

protected:
    virtual uint64_t address() = 0;
    virtual uint64_t length() = 0;

    virtual uint64_t getValueInt(std::string const &name) = 0;
    virtual double getValueFloat(std::string const &name) = 0;
};

} // namespace GenICam

} // namespace Jgv

#endif // INODE_H
