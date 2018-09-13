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

#ifndef IINTEGER_H
#define IINTEGER_H

#include "iinterface.h"

namespace Jgv {

namespace GenICam {

namespace Integer {

class Interface : public GenICam::Interface
{
public:
    virtual ~Interface() = default;
    /*!
         * \brief
         * Cette propriété contient la valeur de l'inode.
         * \return Un entier 64 bits non signé.
         */
    virtual uint64_t getValue() = 0;
    /*!
         * \brief
         * Enregistre une copie de value sur l'inode.
         * \param value L'entier 64 bits non signé à enregistrer.
         */
    virtual void setValue(uint64_t value) = 0;
    /*!
         * \brief
         * Cette propriété contient la valeur minimale de l'inode.
         * Si l'inode ne porte pas cette information, la valeur retournée est la minimum du type uint64_t.
         * \return Un entier 64 bits non signé.
         */
    virtual uint64_t getMin() = 0;
    /*!
         * \brief
         * Cette propriété contient la valeur minimale de l'inode.
         * Si l'inode ne porte pas cette information, la valeur retournée est la maximum du type uint64_t.
         * \return Un entier 64 bits non signé.
         */
    virtual uint64_t getMax() = 0;
    /*!
         * \brief
         * Cette propriété contient la valeur minimale de l'inode.
         * Si l'inode ne porte pas cette information, la valeur retournée est 0.
         * \return Un entier 64 bits non signé.
         */
    virtual uint64_t getInc() = 0;

    Type interfaceType() const override final {
        return Type::IInteger;
    }

    std::string interfaceTypeString() const override final {
        return "IInteger";
    }

}; // class Interface

} // namespace Integer

} // namespace GenICam

} // namespace Jgv

#endif // IINTEGER_H
