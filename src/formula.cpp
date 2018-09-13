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
#include "formula.h"
#include "token.h"
#include "logger.h"
#include <cctype>

#include <stack>
#include <cmath>

#include <boost/algorithm/string.hpp>

using namespace Jgv::GenICam;

Formula::Formula(const std::string &formula)
    : _formula(formula)
{
    std::stack<Token::Object> stack;

    int position = 0;
    while (position < _formula.size()) {

        Token::Object token = getNextToken(position);
        position += token.getSize();

        // cas nombre
        if (token.isNumber()) {
            output.push(token);
        }

        // cas opérateur
        else if (token.isOperator()) {
            // traitement des autres token GenICam
            while (!stack.empty()             // la pile n'est pas vide
                   && stack.top().isOperator()  // le haut de la pile contient un opérateur
                   && (
                       // associativité gauche droite et priorité égale
                       (token.isLeftAssociative() && predecendenceIsEqual(token, stack.top()))
                       // priorité plus faible
                       || predecendenceIsLess(token, stack.top()))
                   )
            {
                // on dépile la pile vers la sortie
                output.push(stack.top());
                stack.pop();
            }

            // le traitement de la pile est fini, on empile le token
            stack.push(token);
        }

        // cas parenthèse gauche
        else if (token.isLeftParenthesis()) {
            stack.push(token);      // parenthèse gauche toujours sur la pile
        }

        // cas parenthèse droite
        else if (token.isRightParenthesis()) {
            bool leftParenthesisFounded = false;

            // on traite la pile
            while (!stack.empty() ) {
                // tant qu'on ne trouve pas une parenthèse gauche, on dépile
                if (!stack.top().isLeftParenthesis()) {
                    // on empile les opérateurs
                    output.push(stack.top());
                    stack.pop();
                }
                else {
                    // on supprime juste la parenthèse gauche
                    stack.pop();
                    leftParenthesisFounded = true;
                    break;
                }

            }
            if (!leftParenthesisFounded) {
                lyu_warning("ERROR : Left parenthesis not found on the stack !");
                while (!stack.empty())
                    stack.pop();
                debugOutput("_Formula::setFormula (!leftParenthesisFounded)");
                break;
            }
        }
    }


    // tous les tokens on été lu, on pousse la pile vers la sortie
    while (!stack.empty()) {
        // si il reste des parenthèses sur la pile, on a une erreur
        if (stack.top().isLeftParenthesis() || stack.top().isRightParenthesis()) {
            lyu_warning("ERROR : Parenthesis left on the stack !");
            debugOutput("Formula::setFormula (stack.top().isLeftParenthesis() || stack.top().isRightParenthesis())");
            break;
        }
        else {
            output.push(stack.top());
            stack.pop();
        }
    }
}

Token::Object Formula::getNextToken(int fromPosition) const
{
    Token::Object token;

    const char current = _formula.at(fromPosition);
    if (current == ' ') {
    }
    else if (current == '(') {
        token.setType(Token::Type::LEFT_BRACKET);
    }
    else if (current == ')') {
        token.setType(Token::Type::RIGHT_BRACKET);
    }
    else if (current == '+') {
        token.setType(Token::Type::ADDITION);
    }
    else if (current == '-') {
        char next = _formula.at(fromPosition + 1);
        // pour que le token soit une soustraction,
        // il faut qu'il soit suivi d'un blanc
        if (next == ' ') {
            token.setType(Token::Type::SUBSTRACTION);
        }
        // dans le cas contraire on doit avoir un nombre négatif
        // si il est directement suivi d'un digit
        else if (isdigit(next)) {
            std::string var = getNumber(fromPosition);
            token.setType(var.find('.') != std::string::npos ?
                          Token::Type::DOUBLE : Token::Type::INT64, var);
        }
    }
    else if (current == '*') {
        if (_formula.at(fromPosition + 1) == '*') {
            token.setType(Token::Type::POWER);
        }
        else {
            token.setType(Token::Type::MULTIPLICATION);
        }
    }
    else if (current == '/') {
        token.setType(Token::Type::DIVISION);
    }
    else if (current == '%') {
        token.setType(Token::Type::REMAINDER);
    }
    else if (current == '&') {
        if (_formula.at(fromPosition + 1) == '&') {
            token.setType(Token::Type::LOGICAL_AND);
        }
        else {
            token.setType(Token::Type::BITWISE_AND);
        }
    }
    else if (current == '|') {
        if (_formula.at(fromPosition + 1) == '|') {
            token.setType(Token::Type::LOGICAL_OR);
        }
        else {
            token.setType(Token::Type::BITWISE_OR);
        }
    }
    else if (current == '^') {
        token.setType(Token::Type::BITWISE_XOR);
    }
    else if (current == '~') {
        token.setType(Token::Type::BITWISE_NOT);
    }
    else if (current == '<') {
        const char next = _formula.at(fromPosition + 1);
        if (next == '>') {
            token.setType(Token::Type::LOGICAL_NOT_EQUAL);
        }
        else if (next == '=') {
            token.setType(Token::Type::LOGICAL_LESS_OR_EQUAL);
        }
        else if (next == '<') {
            token.setType(Token::Type::SHIFT_LEFT);
        }
        else {
            token.setType(Token::Type::LOGICAL_LESS);
        }
    }
    else if (current == '>') {
        const char next = _formula.at(fromPosition + 1);
        if (next == '=') {
            token.setType(Token::Type::LOGICAL_GREATER_OR_EQUAL);
        }
        else if (next == '>') {
            token.setType(Token::Type::SHIFT_RIGHT);
        }
        else {
            token.setType(Token::Type::LOGICAL_GREATER);
        }
    }
    else if (current == '=') {
        token.setType(Token::Type::LOGICAL_EQUAL);
    }
    else if (current == '?') {
        token.setType(Token::Type::TERNARY_QUESTION_MARK);
    }
    else if (current == ':') {
        token.setType(Token::Type::TERNARY_COLON);
    }

    else if (boost::iequals(_formula.substr(fromPosition, 2).c_str(), "lg")) {
        token.setType(Token::Type::LG);
    }

    // le token est un chiffre
    // on est dans le cas d'une valeur décimale ou hexa
    else if (isdigit(current)) {
        std::string var = getNumber(fromPosition);
        token.setType(var.find('.') != std::string::npos ?
                      Token::Type::DOUBLE : Token::Type::INT64, var);
    }

    // le dernier cas possible est d'avoir une variable nommée
    else {
        token = getVariable(fromPosition);
    }

    return token;
}

Token::Object Formula::getVariable(int fromPosition) const
{
    // token nul
    Token::Object ret;

    // une variable commence par une lettre
    if (isalpha(_formula.at(fromPosition))) {
        int pos = fromPosition + 1;
        while (pos < _formula.size()) {
            const char c = _formula.at(pos);
            // una variable contient lettres, chiffres, underscore ou point
            if ( !(isalnum(c) || (c == '_')  || (c == '.')) )
            { break;}
            ++pos;
        }
        std::string var = _formula.substr(fromPosition, pos - fromPosition);
        ret.setType(Token::Type::VARIABLE, var);
    }
    return ret;
}

std::string Formula::getNumber(int fromPosition) const
{
    int index = fromPosition;

    // pour les nombres négatifs
    if (_formula.at(index) == '-') {
        ++index;
    }

    while (index < _formula.size()) {
        if (isalnum(_formula.at(index)) || _formula.at(index) == '.') {
            ++index;
        }
        else {
            break;
        }
    }
    return _formula.substr(fromPosition, index - fromPosition);
}

bool Formula::predecendenceIsEqual(const Token::Object &left, const Token::Object &right) const
{
    return (left.precedence() == right.precedence());
}

bool Formula::predecendenceIsLess(const Token::Object &left, const Token::Object &right) const
{
    return (left.precedence() < right.precedence());
}

void Formula::debugOutput(const std::string &additionnalInfos) const
{
    std::queue<Token::Object> tmpoutput(output);
    std::string rpn;
    while (!tmpoutput.empty()) {
        Token::Object &it = tmpoutput.front();
        std::string post;
        if (it.isINT64()) {
            post = std::to_string(it.toInteger());
        }
        else if (it.isDOUBLE()) {
            post = std::to_string(it.toFloat());
        }
        rpn.append(it.getGenicamToken() + post + " ");
        tmpoutput.pop();
    }
    lyu_debug("------------ Formula DEBUG ------------");
    // lyu_debug("source : %s", qPrintable(additionnalInfos));
    auto fit = _floatVariables.begin();
    while (fit != _floatVariables.end()) {
        lyu_debug("FLOAT VAR: " << fit->first << " " << fit->second);
        ++fit;
    }
    auto iit = _intVariables.begin();
    while (iit != _intVariables.end()) {
        lyu_debug("INT VAR: " << iit->first << " " << iit->second);
        ++iit;
    }

    lyu_debug("infix " << _formula);
    lyu_debug("outfix" << rpn);
    lyu_debug("------------ END DEBUG ------------");
}

std::vector<std::string> Formula::variablesList() const
{
    std::vector<std::string> list;
    std::queue<Token::Object> tmpoutput(output);

    while (!tmpoutput.empty()) {
        Token::Object &it = tmpoutput.front();
        if (it.isVariable())
            if (std::find(list.begin(), list.end(), it.getGenicamToken()) == list.end())
                list.push_back(it.getGenicamToken());
        tmpoutput.pop();
    }

    return list;
}

int64_t Formula::evaluateAsInteger(const IntegerVariables &intVariables,
                                   const FloatVariables &floatVariables)
{
    _intVariables = intVariables;
    _floatVariables = floatVariables;

    std::stack<Token::Object> stack;
    std::queue<Token::Object> tmpoutput(output);
    int64_t result = 0;
    int64_t o1=0., o2=0., o3=0.;


    while (!tmpoutput.empty()) {
        Token::Object *it = &tmpoutput.front();
        // variable on renseigne la valeur et on pousse
        if ( (*it).isVariable() ) {
            std::string genicamToken = (*it).getGenicamToken();
            if (intVariables.find(genicamToken) != intVariables.end()) {
                int64_t value = _intVariables[genicamToken];
                (*it).setInteger(value);
            }
            // variable int64 on renseigne la valeur et on pousse
            else if (floatVariables.find(genicamToken) != floatVariables.end()) {
                double value = _floatVariables[genicamToken];
                (*it).setFloat(value);
            }
            else {
                lyu_critical("Formula::evaluateAsInteger variable inconnue" << genicamToken);
            }

            stack.push(*it);
        }
        else if ((*it).isNumber()) {
            stack.push(*it);
        }
        else {
            const int args = (*it).operandsCount();
            if (stack.size() < args) {
                lyu_debug("error evaluate stack size");
                debugOutput("Formula::evaluate() (stack.size() < args)");
                break;
            }

            Token::Object shortEval;
            shortEval.setType(Token::Type::INT64);

            if (args >= 1) {
                o1 = stack.top().toInteger();
                stack.pop();
            }
            if (args >= 2) {
                o2 = stack.top().toInteger();
                stack.pop();
            }
            if (args == 3) {
                o3 = stack.top().toInteger();
                stack.pop();
            }

            switch ((*it).getType()) {
            case Token::Type::ADDITION:
                shortEval.setInteger(o2 + o1);
                break;
            case Token::Type::SUBSTRACTION:
                shortEval.setInteger(o2 - o1);
                break;
            case Token::Type::MULTIPLICATION:
                shortEval.setInteger(o2 * o1);
                break;
            case Token::Type::DIVISION:
                if (o1 != 0) {
                    shortEval.setInteger(o2 / o1);
                }
                break;
            case Token::Type::REMAINDER:
                if (o1 != 0) {
                    shortEval.setInteger(o2 % o1);
                }
                break;
            case Token::Type::POWER:
                shortEval.setInteger(pow(o2, o1));
                break;
            case Token::Type::BITWISE_AND:
                shortEval.setInteger(o2 & o1);
                break;
            case Token::Type::BITWISE_OR:
                shortEval.setInteger(o2 | o1);
                break;
            case Token::Type::BITWISE_XOR:
                shortEval.setInteger(o2 ^ o1);
                break;
            case Token::Type::BITWISE_NOT:
                shortEval.setInteger(~o1);
                break;
            case Token::Type::LOGICAL_NOT_EQUAL:
                shortEval.setInteger(o2 != o1);
                break;
            case Token::Type::LOGICAL_EQUAL:
                shortEval.setInteger(o2 == o1);
                break;
            case Token::Type::LOGICAL_GREATER:
                shortEval.setInteger(o2 > o1);
                break;
            case Token::Type::LOGICAL_LESS:
                shortEval.setInteger(o2 < o1);
                break;
            case Token::Type::LOGICAL_LESS_OR_EQUAL:
                shortEval.setInteger(o2 <= o1);
                break;
            case Token::Type::LOGICAL_GREATER_OR_EQUAL:
                shortEval.setInteger(o2 >= o1);
                break;
            case Token::Type::LOGICAL_AND:
                shortEval.setInteger(o2 && o1);
                break;
            case Token::Type::LOGICAL_OR:
                shortEval.setInteger(o2 || o1);
                break;
            case Token::Type::SHIFT_LEFT:
                shortEval.setInteger(o2 << o1);
                break;
            case Token::Type::SHIFT_RIGHT:
                shortEval.setInteger(o2 >> o1);
                break;
            case Token::Type::TERNARY_QUESTION_MARK:
                shortEval.setInteger(o3 ? o2 : o1);
                break;
            case Token::Type::TERNARY_COLON:
                shortEval.setInteger(o1);
                break;
            default:
                lyu_warning("Formula::evaluateAsInteger: Token not evaluate");
            }

            stack.push(shortEval);
        }

        tmpoutput.pop();
    }
    if (stack.size() == 1) {
        result = stack.top().toFloat();
        stack.pop();
    }
    else {
        lyu_debug("error evaluate as integer result stack");
    }

    return result;
}

double Formula::evaluateAsFloat(const IntegerVariables &intVariables,
                                const FloatVariables &floatVariables)
{
    _intVariables = intVariables;
    _floatVariables = floatVariables;

    std::stack<Token::Object> stack;
    std::queue<Token::Object> tmpoutput(output);
    bool ok = true;
    double result = 0;
    double o1=0., o2=0., o3=0.;

    while (!tmpoutput.empty()) {
        Token::Object *it = &tmpoutput.front();
        // variable on renseigne la valeur, on change le type de token et on pousse
        if ( (*it).isVariable() ) {
            std::string genicamToken = (*it).getGenicamToken();
            if (intVariables.find(genicamToken) != intVariables.end()) {
                int64_t value = _intVariables[genicamToken];
                (*it).setInteger(value);
            }
            // variable int64 on renseigne la valeur et on pousse
            else if (floatVariables.find(genicamToken) != floatVariables.end()) {
                double value = _floatVariables[genicamToken];
                (*it).setFloat(value);
            }
            else {
                lyu_critical("Formula::evaluateAsDouble variable inconnue " << genicamToken);
            }

            stack.push(*it);
        }
        else if ((*it).isNumber()) {
            stack.push(*it);
        }
        else {
            const int args = (*it).operandsCount();
            if (stack.size() < args) {
                debugOutput("Formula::evaluateAsDouble()");
                break;
            }

            Token::Object shortEval;
            shortEval.setType(Token::Type::DOUBLE);

            if (args >= 1) {
                o1 = stack.top().toFloat();
                stack.pop();
            }
            if (args >= 2) {
                o2 = stack.top().toFloat();
                stack.pop();
            }
            if (args == 3) {
                o3 = stack.top().toFloat();
                stack.pop();
            }

            switch ((*it).getType()) {
            case Token::Type::ADDITION:
                shortEval.setFloat(o2 + o1);
                break;
            case Token::Type::SUBSTRACTION:
                shortEval.setFloat(o2 - o1);
                break;
            case Token::Type::MULTIPLICATION:
                shortEval.setFloat(o2 * o1);
                break;
            case Token::Type::DIVISION:
                if (o1 != 0.) {
                    shortEval.setFloat(o2 / o1);
                }
                break;
            case Token::Type::REMAINDER:
                lyu_warning("REMAINDER on DOUBLE");
                ok = false;
                break;
            case Token::Type::POWER:
                shortEval.setFloat(pow(o2, o1));
                break;
            case Token::Type::BITWISE_AND:
                lyu_warning("BITWISE_AND on DOUBLE");
                ok = false;
                break;
            case Token::Type::BITWISE_OR:
                lyu_warning("BITWISE_OR on DOUBLE");
                ok = false;
                break;
            case Token::Type::BITWISE_XOR:
                lyu_warning("BITWISE_XOR on DOUBLE");
                ok = false;
                break;
            case Token::Type::BITWISE_NOT:
                lyu_warning("BITWISE_NOT on DOUBLE");
                ok = false;
                break;
            case Token::Type::LOGICAL_NOT_EQUAL:
                shortEval.setFloat(o2 != o1);
                break;
            case Token::Type::LOGICAL_EQUAL:
                shortEval.setFloat(o2 == o1);
                break;
            case Token::Type::LOGICAL_GREATER:
                shortEval.setFloat(o2 > o1);
                break;
            case Token::Type::LOGICAL_LESS:
                shortEval.setFloat(o2 < o1);
                break;
            case Token::Type::LOGICAL_LESS_OR_EQUAL:
                shortEval.setFloat(o2 <= o1);
                break;
            case Token::Type::LOGICAL_GREATER_OR_EQUAL:
                shortEval.setFloat(o2 >= o1);
                break;
            case Token::Type::LOGICAL_AND:
                shortEval.setFloat(o2 && o1);
                break;
            case Token::Type::LOGICAL_OR:
                shortEval.setFloat(o2 || o1);
                break;
            case Token::Type::SHIFT_LEFT:
                lyu_warning("SHIFT_LEFT on DOUBLE");
                ok = false;
                break;
            case Token::Type::SHIFT_RIGHT:
                lyu_warning("SHIFT_LEFT on DOUBLE");
                ok = false;
                break;
            case Token::Type::TERNARY_QUESTION_MARK:
                shortEval.setFloat(o3 ? o2 : o1);
                break;
            case Token::Type::TERNARY_COLON:
                shortEval.setFloat(o1);
                break;
            case Token::Type::LG:
                shortEval.setFloat(log10(o1));
                break;
            default:
                lyu_warning("Formula::evaluateAsDouble: Token not evaluate");
            }

            if (ok) {
                stack.push(shortEval);
            }
            else {
                debugOutput("Evaluate");
                break;
            }
        }

        tmpoutput.pop();
    }
    if (stack.size() == 1) {
        result = stack.top().toFloat();
        stack.pop();
    }
    else {
        lyu_debug("error evaluate as double result stack");
    }

    return result;
}
