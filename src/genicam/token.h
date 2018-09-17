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

#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <cstdint>

namespace Jgv {

namespace GenICam {

namespace Token {

enum class Type {
    UNKNOW,
    LEFT_BRACKET, RIGHT_BRACKET,
    ADDITION, SUBSTRACTION, MULTIPLICATION, DIVISION,
    REMAINDER,
    POWER,
    BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_NOT,
    LOGICAL_NOT_EQUAL, LOGICAL_EQUAL, LOGICAL_GREATER, LOGICAL_LESS, LOGICAL_LESS_OR_EQUAL, LOGICAL_GREATER_OR_EQUAL,
    LOGICAL_AND, LOGICAL_OR,
    SHIFT_LEFT, SHIFT_RIGHT,
    TERNARY_QUESTION_MARK, TERNARY_COLON,
    SGN, NEG,
    ATAN, COS, SIN, TAN, ABS, EXP, LG, LN, SQRT, TRUNC, FLOOR, CEIL, ROUND, ASIN, ACOS,
    E, PI,
    INT64, DOUBLE,
    VARIABLE, VARIABLE_INT64, VARIABLE_DOUBLE
};

class Object final
{
    Type m_type = Type::UNKNOW;
    std::string m_genicamToken;
    union Value {
        double floatValue;
        int64_t intValue;
    };
    Value m_value = {};

public:
    void setType(Type type, const std::string &token = std::string()) noexcept;
    Type getType() const noexcept;
    std::string getGenicamToken() const noexcept;
    int getSize() const noexcept;
    bool isNumber() const noexcept;
    bool isVariable() const noexcept;
    bool isINT64() const noexcept;
    bool isDOUBLE() const noexcept;
    bool isOperator() const noexcept;
    bool isLeftParenthesis() const noexcept;
    bool isRightParenthesis() const noexcept;
    bool isLeftAssociative() const noexcept;
    int operandsCount() const noexcept;
    int precedence() const noexcept;
    void setFloat(double value) noexcept;
    double toFloat() const noexcept;
    void setInteger(int64_t value) noexcept;
    int64_t toInteger() const noexcept;

}; // class Object

} // namespace Token

} // namespace GenICam

} // namespace Jgv

#endif // TOKEN_H
