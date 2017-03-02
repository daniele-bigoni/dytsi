/*
 * This file is part of DYTSI.
 *
 * DYTSI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DYTSI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DYTSI.  If not, see <http://www.gnu.org/licenses/>.
 *
 * DYnamics Train SImulation (DYTSI)
 * Copyright (C) 2011-2014 The Technical University of Denmark
 * Scientific Computing Section
 * Department of Applied Mathematics and Computer Science
 *
 * Author: Daniele Bigoni
 * E-mail: dabi@dtu.dk
 *
 */

/* 
 * File:   XMLaux.h
 * Author: bigo
 *
 * Created on March 12, 2011, 4:07 PM
 */

#ifndef XMLAUX_H
#define	XMLAUX_H

#include "Range.h"

#include "../../GeneralModel/PieceWiseValues.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include <vector>
#include <string>

class XMLaux {
public:
    XMLaux();
    virtual ~XMLaux();

    static bool setRange(XERCES_CPP_NAMESPACE::DOMElement* node, 
                const char* startValLabel, const char* endValLabel, const char* stepSizeLabel,
                DYTSI_Input::Range* range);
    static bool loadStringProperty(XERCES_CPP_NAMESPACE::DOMElement* el,
                const char* propertyName, std::string &prop);
    static bool loadRSGEOProperty(XERCES_CPP_NAMESPACE::DOMElement* el,
                const char* propertyName, XERCES_CPP_NAMESPACE::DOMNode* &node);
    static bool loadDoubleProperty(XERCES_CPP_NAMESPACE::DOMElement* el,
                const char* propertyName, double &prop);
    static bool loadPWLValues(XERCES_CPP_NAMESPACE::DOMElement* el,
                const char* propertyName, DYTSI_Modelling::PieceWiseValues* &pwv);
    static bool loadElementProperty(XERCES_CPP_NAMESPACE::DOMElement* el,
                const char* propertyName, XERCES_CPP_NAMESPACE::DOMElement* &prop);
    static double setValueToDouble(XERCES_CPP_NAMESPACE::DOMNode* node);
    static std::string setValueToString(XERCES_CPP_NAMESPACE::DOMNode* node);
    static bool loadStringAttribute(XERCES_CPP_NAMESPACE::DOMElement* el,
                const char* attributeName, std::string &attr);

    static void getElementsNodeByName(
                std::vector<XERCES_CPP_NAMESPACE::DOMNode*>& elementsVector,
                XERCES_CPP_NAMESPACE::DOMNode* node, const char* name);

    static int serializeDOM(XERCES_CPP_NAMESPACE::DOMNode* node, std::stringstream* output);

private:

};

#endif	/* XMLAUX_H */

