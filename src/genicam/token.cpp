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

#include "token.h"

#include <string>

using namespace Jgv::GenICam::Token;

namespace {

enum class Associativity {
    LEFT_TO_RIGHT,
    RIGHT_TO_LEFT
};

struct Info {
    int precedence;
    Associativity associativity;
    int operandsCount;
    std::string geniCamToken;
};

Info INFOS[] = {
    {     0,     Associativity::LEFT_TO_RIGHT,       1,        "#"     },  // UNKNOW
    {    -1,     Associativity::LEFT_TO_RIGHT,       0,        "("     },  // LEFT_BRACKET
    {   990,     Associativity::LEFT_TO_RIGHT,       0,        ")"     },  // RIGHT_BRACKET
    {   100,     Associativity::LEFT_TO_RIGHT,       2,        "+"     },  // ADDITION
    {   100,     Associativity::LEFT_TO_RIGHT,       2,        "-"     },  // SUBSTRACTION
    {   110,     Associativity::LEFT_TO_RIGHT,       2,        "*"     },  // MULTIPLICATION
    {   110,     Associativity::LEFT_TO_RIGHT,       2,        "/"     },  // DIVISION
    {   110,     Associativity::LEFT_TO_RIGHT,       2,        "%"     },  // REMAINDER
    {   120,     Associativity::RIGHT_TO_LEFT,       2,        "**"    },  // POWER
    {    60,     Associativity::LEFT_TO_RIGHT,       2,        "&"     },  // BITWISE_AND
    {    40,     Associativity::LEFT_TO_RIGHT,       2,        "|"     },  // BITWISE_OR
    {    50,     Associativity::LEFT_TO_RIGHT,       2,        "^"     },  // BITWISE_XOR
    {    30,     Associativity::LEFT_TO_RIGHT,       1,        "~"     },  // BITWISE_NOT
    {    70,     Associativity::LEFT_TO_RIGHT,       2,        "<>"    },  // LOGICAL_NOT_EQUAL
    {    70,     Associativity::LEFT_TO_RIGHT,       2,        "="     },  // LOGICAL_EQUAL
    {    80,     Associativity::LEFT_TO_RIGHT,       2,        ">"     },  // LOGICAL_GREATER
    {    80,     Associativity::LEFT_TO_RIGHT,       2,        "<"     },  // LOGICAL_LESS
    {    80,     Associativity::LEFT_TO_RIGHT,       2,        "<="    },  // LOGICAL_LESS_OR_EQUAL
    {    80,     Associativity::LEFT_TO_RIGHT,       2,        ">="    },  // LOGICAL_GREATER_OR_EQUAL
    {    20,     Associativity::LEFT_TO_RIGHT,       2,        "&&"    },  // LOGICAL_AND
    {    10,     Associativity::LEFT_TO_RIGHT,       2,        "||"    },  // LOGICAL_OR
    {    90,     Associativity::LEFT_TO_RIGHT,       2,        "<<"    },  // SHIFT_LEFT
    {    90,     Associativity::LEFT_TO_RIGHT,       2,        ">>"    },  // SHIFT_RIGHT
    {     5,     Associativity::RIGHT_TO_LEFT,       3,        "?"     },  // TERNARY_QUESTION_MARK
    {     5,     Associativity::RIGHT_TO_LEFT,       1,        ":"     },  // TERNARY_COLON
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "SGN"   },  // SGN
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "NEG"   },  // NEG
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "ATAN"  },  // ATAN
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "COS"   },  // COS
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "SIN"   },  // SIN
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "TAN"   },  // TAN
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "ABS"   },  // ABS
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "EXP"   },  // EXP
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "LN"    },  // LN
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "SQRT"  },  // SQRT
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "TRUNC" },  // TRUNC
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "FLOOR" },  // FLOOR
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "CEIL"  },  // CEIL
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "ROUND" },  // ROUND
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "ASIN"  },  // ASIN
    {   200,     Associativity::LEFT_TO_RIGHT,       1,        "ACOS"  },  // ACOS
    {   200,     Associativity::LEFT_TO_RIGHT,       0,        "E"     },  // E
    {   200,     Associativity::LEFT_TO_RIGHT,       0,        "PI"    },  // PI
    {   200,     Associativity::LEFT_TO_RIGHT,       0,        ""      },  // INT64
    {   200,     Associativity::LEFT_TO_RIGHT,       0,        ""      },  // DOUBLE
    {   200,     Associativity::LEFT_TO_RIGHT,       0,        ""      },  // INT64_VARIABLE
    {   200,     Associativity::LEFT_TO_RIGHT,       0,        ""      }   // DOUBLE_VARIABLE
};

} // anonymous namespace

void Object::setType(Token::Type type, const std::string &token) noexcept
{
    m_type = type;
    // une valeur sous forme de bytearray décrit le Token
    if (!token.empty()) {
        m_genicamToken = token;
        if (m_type == Type::INT64) {
            m_value.intValue = std::stoll(m_genicamToken);
        }
        else if (type == Type::DOUBLE) {
            m_value.floatValue = std::stod(m_genicamToken);
        }
    }
}

Type Object::getType() const noexcept
{
    return m_type;
}

std::string Object::getGenicamToken() const noexcept
{
    return isNumber() ? m_genicamToken : INFOS[static_cast<int>(m_type)].geniCamToken;
}

int Object::getSize() const noexcept
{
    if (m_type <= Type::PI) {
        return INFOS[static_cast<int>(m_type)].geniCamToken.size();
    }
    else {
        return m_genicamToken.size();
    }
}

bool Object::isNumber() const noexcept
{
    return (m_type >= Type::E);
}

bool Object::isVariable() const noexcept
{
    return (m_type >=Type::VARIABLE);
}

bool Object::isINT64() const noexcept
{
    return (m_type==Type::INT64 || m_type==Type::VARIABLE_INT64);
}

bool Object::isDOUBLE() const noexcept
{
    return (m_type==Type::DOUBLE || m_type==Type::VARIABLE_DOUBLE);
}

bool Object::isOperator() const noexcept
{
    return (m_type >= Type::ADDITION) && (m_type <= Type::ACOS);
}

bool Object::isLeftParenthesis() const
noexcept{
    return (m_type == Type::LEFT_BRACKET);
}

bool Object::isRightParenthesis() const noexcept
{
    return (m_type == Type::RIGHT_BRACKET);
}

bool Object::isLeftAssociative() const noexcept
{
    return (INFOS[static_cast<int>(m_type)].associativity == Associativity::LEFT_TO_RIGHT);
}

int Object::operandsCount() const noexcept
{
    return INFOS[static_cast<int>(m_type)].operandsCount;
}

int Object::precedence() const noexcept
{
    return INFOS[static_cast<int>(m_type)].precedence;
}

void Object::setFloat(double value) noexcept
{
    // si variable non typée, on type
    if (m_type == Type::VARIABLE) {
        m_type = Type::VARIABLE_DOUBLE;
    }
    m_value.floatValue = value;
}

double Object::toFloat() const noexcept
{
    return isDOUBLE() ? m_value.floatValue : static_cast<double>(m_value.intValue);
}

void Object::setInteger(int64_t value) noexcept
{
    // si variable non typée, on type
    if (m_type == Type::VARIABLE) {
        m_type = Type::VARIABLE_INT64;
    }
    m_value.intValue = value;
}

int64_t Object::toInteger() const noexcept
{
    return isINT64() ? m_value.intValue : static_cast<int64_t>(m_value.floatValue);
}
