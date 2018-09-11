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

#ifndef GENICAMXMLFILE_H
#define GENICAMXMLFILE_H

#include <vector>
#include <string>
#include <memory>
#include <map>

#include <Poco/RefPtr.h>
#include <Poco/RefCountedObject.h>
#include <Poco/DOM/Document.h>

namespace Poco {
    namespace XML {
        class NodeList;
        class Node;
    }
}

namespace Jgv {

namespace GenICam {

class GenICamXMLParserImpl;
class GenICamXMLParser : public Poco::RefCountedObject
{
public:
    typedef Poco::RefPtr<Poco::XML::Document> DomPtr;
    typedef Poco::RefPtr<Poco::XML::NodeList> NodeListPtr;
    typedef std::map<std::string, Poco::XML::Element*> NodeIndexMap;

    virtual ~GenICamXMLParser() = default;
    void parse(std::string const &xml);

    Poco::XML::Element* getNodeByNameAttribute(const std::string &name);

private:
    void createIndex();
    void fillIndex(Poco::XML::Node *node);

    DomPtr _dom;
    NodeIndexMap _index;
}; // class GenICamXML

} // namespace GenICam

} // namespace Jgv

#endif // GENICAMXMLFILE_H
