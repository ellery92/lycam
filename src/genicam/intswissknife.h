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

#ifndef INTSWISSKNIFE_H
#define INTSWISSKNIFE_H
#include "iinteger.h"
#include "node.h"

namespace Jgv {

namespace GenICam {

class IntSwissKnifeNode final : public Node, public Integer::Interface
{
public:
    IntSwissKnifeNode(std::string const &name, GenICamXMLParser::WeakPtr xmlParser,
                      IPort::Interface::WeakPtr port)
        : Node(name, xmlParser, port) {}

    virtual ~IntSwissKnifeNode() = default;
    virtual uint64_t getValue() override;
    virtual void setValue(uint64_t value) override;
    virtual uint64_t getMin() override { return 0; }
    virtual uint64_t getMax() override { return 0; }
    virtual uint64_t getInc() override { return 0; };

    virtual Interface *interface() override { return this; }
}; // class Object

} // namespace GenICam

} // namespace Jgv

#endif // INTSWISSKNIFE_H
