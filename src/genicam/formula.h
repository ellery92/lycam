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
#ifndef FORMULA_H
#define FORMULA_H

#include <string>
#include <map>
#include <queue>
#include "token.h"

namespace Jgv {

namespace GenICam {

using IntegerVariables = std::map<std::string, int64_t>;
using FloatVariables = std::map<std::string, double>;

class Formula final
{
public:
    Formula(const std::string &formula);
    ~Formula() = default;

    std::vector<std::string> variablesList() const;

    int64_t evaluateAsInteger(const IntegerVariables &intVariables,
                              const FloatVariables &floatVariables);
    double evaluateAsFloat(const IntegerVariables &intVariables,
                           const FloatVariables &floatVariables);

    void debugOutput(const std::string &additionnalInfos) const;

private:
    const std::string _formula;
    IntegerVariables _intVariables;
    FloatVariables _floatVariables;
    std::queue<Token::Object> output;

    Token::Object getNextToken(int fromPosition) const;
    Token::Object getVariable(int fromPosition) const;
    std::string getNumber(int fromPosition) const;
    bool predecendenceIsEqual(const Token::Object &left, const Token::Object &right) const;
    bool predecendenceIsLess(const Token::Object &left, const Token::Object &right) const;
}; // class Formula

} // namespace GenICam

} // namespace Jgv

#endif // FORMULA_H
