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

#include "genicamxmlparser.h"

#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Node.h>
#include <Poco/DOM/Document.h>
#include <Poco/AutoPtr.h>

#include "logger.h"

using namespace Jgv::GenICam;

constexpr const char *DESCRIPTION = "RegisterDescription";
constexpr const char *GROUP = "Group";
constexpr const char *STRUCTREG = "StructReg";
constexpr const char *STRUCTENTRY = "StructEntry";

class GenICamXMLParserImpl final : public GenICamXMLParser
{
public:
    typedef Poco::AutoPtr<Poco::XML::Document> DomPtr;
    typedef Poco::AutoPtr<Poco::XML::NodeList> NodeListPtr;
    typedef std::map<std::string, Poco::XML::Element*> NodeIndexMap;

    virtual ~GenICamXMLParserImpl() = default;
    virtual void parse(std::string const &xml) override;
    virtual Poco::XML::Element* getNodeByNameAttribute(const std::string &name) override;

private:
    void createIndex();
    void fillIndex(Poco::XML::Node *node);

    DomPtr _dom;
    NodeIndexMap _index;
}; // class GenICamXML

void GenICamXMLParserImpl::parse(std::string const &xml)
{
    Poco::XML::DOMParser domParser;
    _dom = domParser.parseString(xml);

    createIndex();
}

void GenICamXMLParserImpl::createIndex()
{
    _index.clear();

    NodeListPtr registerDescription = _dom->getElementsByTagName(DESCRIPTION);

    if (registerDescription->length() != 1) {
        lyu_warning("xml parse: No Root value");
        return;
    }

    // parcourt les noeuds à la racine
    fillIndex(registerDescription->item(0));
}

void GenICamXMLParserImpl::fillIndex(Poco::XML::Node *node)
{
    if (node->nodeType() == Poco::XML::Node::ELEMENT_NODE) {
        Poco::XML::Element *elem = static_cast<Poco::XML::Element*>(node);
        std::string name = elem->getAttribute("Name");
        if (!name.empty()) {
            _index[name] = elem;
        }
    }

    if (!node->hasChildNodes())
        return;

    Poco::XML::Node *p = node->firstChild();
    while (p) {
        fillIndex(p);
        p = p->nextSibling();
    }
}

Poco::XML::Element *GenICamXMLParserImpl::getNodeByNameAttribute(const std::string &name)
{
    NodeIndexMap::const_iterator it = _index.find(name);
    if (it == _index.end()) {
        lyu_debug("there is no node whos name is " << name);
        poco_assert(false);
        return NULL;
    }

    return it->second;
}

GenICamXMLParser::Ptr GenICamXMLParser::create()
{
    return std::make_shared<GenICamXMLParserImpl>();
}
