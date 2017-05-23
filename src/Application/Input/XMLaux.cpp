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
 * E-mail: dabi@limitcycle.it
 *
 */

/* 
 * File:   XMLaux.cpp
 * Author: bigo
 * 
 * Created on March 12, 2011, 4:07 PM
 */

#include "XMLaux.h"

#include "../miscellaneous.h"
#include "../PrintingHandler.h"

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>

#include <sstream>
#include <iostream>
#include <iterator>

XMLaux::XMLaux() {
}

XMLaux::~XMLaux() {
}

void XMLaux::getElementsNodeByName(
            std::vector<XERCES_CPP_NAMESPACE::DOMNode*> &elementsVector,
            XERCES_CPP_NAMESPACE::DOMNode* node, const char* name){
    XERCES_CPP_NAMESPACE::DOMNode* child;
    for (child = node->getFirstChild(); child != 0; child = child->getNextSibling()){
        if (child->getNodeType() == XERCES_CPP_NAMESPACE::DOMNode::ELEMENT_NODE){

            char* nodeName = XERCES_CPP_NAMESPACE::XMLString::transcode(child->getNodeName());
            if  (strcmp(nodeName, name) == 0){
                elementsVector.push_back(child);
            }
        }
    }
}

double XMLaux::setValueToDouble(xercesc_3_1::DOMNode* node){
    char* value = XERCES_CPP_NAMESPACE::XMLString::transcode(node->getFirstChild()->getNodeValue());
    return atof(value);
}

std::string XMLaux::setValueToString(xercesc_3_1::DOMNode* node){
    char* value = XERCES_CPP_NAMESPACE::XMLString::transcode(node->getFirstChild()->getNodeValue());
    return std::string(value);
}

bool XMLaux::loadStringProperty(
        XERCES_CPP_NAMESPACE::DOMElement* el, const char* propertyName, std::string &prop){
    bool error = false;
    XERCES_CPP_NAMESPACE::DOMNodeList* list = el->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode(propertyName));
    if (list->getLength() == 1){
        prop = XMLaux::setValueToString(list->item(0));
    } else {
        error = true;
        std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\t\tError in parsing the input file: " << std::endl;
        *out << "\t\t\tThe property \"" << propertyName << "\" is not well defined";
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
    }
    return error;
}

bool XMLaux::loadRSGEOProperty(XERCES_CPP_NAMESPACE::DOMElement* el,
                const char* propertyName, XERCES_CPP_NAMESPACE::DOMNode* &node) {
    bool error = false;
    XERCES_CPP_NAMESPACE::DOMNodeList* list = el->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode(propertyName));
    if (list->getLength() == 1){
        node = list->item(0);
    } else {
        error = true;
        std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\t\tError in parsing the input file: " << std::endl;
        *out << "\t\t\tThe property \"" << propertyName << "\" is not well defined";
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        return error;
    }
    return error;
}

bool XMLaux::loadDoubleProperty(
        XERCES_CPP_NAMESPACE::DOMElement* el, const char* propertyName, double &prop){
    bool error = false;
    XERCES_CPP_NAMESPACE::DOMNodeList* list = el->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode(propertyName));
    if (list->getLength() == 1){
        XERCES_CPP_NAMESPACE::DOMNode* node = list->item(0);
        prop = XMLaux::setValueToDouble(node);
    } else {
        error = true;
        std::stringstream *out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\t\tError in parsing the input file: " << std::endl;
        *out << "\t\t\tThe property \"" << propertyName << "\" is not well defined";
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
    }
    return error;
}

bool XMLaux::loadPWLValues(XERCES_CPP_NAMESPACE::DOMElement* el,
                const char* propertyName, DYTSI_Modelling::PieceWiseValues* &pwv){
    bool error = false;
    double** vals;
    int n;
    std::string interpStr;
    XERCES_CPP_NAMESPACE::DOMNodeList* list = el->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode(propertyName));
    if (list->getLength() == 1){
        XERCES_CPP_NAMESPACE::DOMElement* node = (XERCES_CPP_NAMESPACE::DOMElement*) list->item(0);

        // Get the interpolation function
        error = XMLaux::loadStringAttribute(node, "Interp", interpStr);

        // Get X and Y
        XERCES_CPP_NAMESPACE::DOMNodeList* listX = node->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode("X"));
        XERCES_CPP_NAMESPACE::DOMNodeList* listY = node->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode("Y"));

        if ((listX->getLength() == 1) && (listY->getLength() == 1)){
            // Read the X and Y list
            XERCES_CPP_NAMESPACE::DOMNode* nodeX = listX->item(0);
            char* x_vals_c = XERCES_CPP_NAMESPACE::XMLString::transcode( nodeX->getTextContent() );
            XERCES_CPP_NAMESPACE::DOMNode* nodeY = listY->item(0);
            char* y_vals_c = XERCES_CPP_NAMESPACE::XMLString::transcode( nodeY->getTextContent() );
            // Parse the values
            std::string x_str = x_vals_c;
            std::stringstream x_strstr(x_str);
            std::istream_iterator<std::string> x_it(x_strstr);
            std::istream_iterator<std::string> x_end;
            std::vector<std::string> x_results(x_it, x_end);
            std::string y_str = y_vals_c;
            std::stringstream y_strstr(y_str);
            std::istream_iterator<std::string> y_it(y_strstr);
            std::istream_iterator<std::string> y_end;
            std::vector<std::string> y_results(y_it, y_end);

            if (x_results.size() == y_results.size()){
                // Allocate space
                doubleMatrixAlloc(vals, 2, x_results.size());

                for(unsigned int i = 0; i < x_results.size(); i++){
                    vals[0][i] = atof(x_results[i].c_str());
                    vals[1][i] = atof(y_results[i].c_str());
                }

                n = x_results.size();

                pwv = new DYTSI_Modelling::PieceWiseValues(vals,n,interpStr);

                return error;
            }
        }
    } 
    error = true;
    std::stringstream *out = new std::stringstream(std::stringstream::in|std::stringstream::out);
    *out << "\t\t\tError in parsing the input file: " << std::endl;
    *out << "\t\t\tThe property \"" << propertyName << "\" is not well defined";
    PrintingHandler::printOut(out,PrintingHandler::STDOUT);
    delete out;
    return error;
}

bool XMLaux::loadElementProperty(XERCES_CPP_NAMESPACE::DOMElement* el,
            const char* propertyName, XERCES_CPP_NAMESPACE::DOMElement* &prop){
    bool error = false;
    XERCES_CPP_NAMESPACE::DOMNodeList* list = el->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode(propertyName));
    if (list->getLength() == 1){
        prop = (XERCES_CPP_NAMESPACE::DOMElement*)list->item(0);
    } else {
        error = true;
        std::stringstream *out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\t\tError in parsing the input file: " << std::endl;
        *out << "\t\t\tThe property \"" << propertyName << "\" is not well defined";
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
    }
    return error;
}

bool XMLaux::loadStringAttribute(XERCES_CPP_NAMESPACE::DOMElement* el,
                const char* attributeName, std::string &attr){
    bool error = false;
    XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = el->getAttributes();
    XERCES_CPP_NAMESPACE::DOMAttr* interpAttribute =
                (XERCES_CPP_NAMESPACE::DOMAttr*)pAttributes->getNamedItem(
                XERCES_CPP_NAMESPACE::XMLString::transcode("Interp"));
    if (interpAttribute != NULL){
        char* interpTypeStr = XERCES_CPP_NAMESPACE::XMLString::transcode(interpAttribute->getValue());
        attr = std::string(interpTypeStr);
    } else
        attr = "";
    return error;
}

bool XMLaux::setRange(XERCES_CPP_NAMESPACE::DOMElement* node,
                const char* startValLabel, const char* endValLabel, const char* stepSizeLabel,
                DYTSI_Input::Range* range){
    bool error = false;
    //range = new DYTSI_Input::Range();

    XERCES_CPP_NAMESPACE::DOMNodeList* startValueList = node->getElementsByTagName(
            XERCES_CPP_NAMESPACE::XMLString::transcode(startValLabel));
    XERCES_CPP_NAMESPACE::DOMNodeList* endValueList = node->getElementsByTagName(
            XERCES_CPP_NAMESPACE::XMLString::transcode(endValLabel));
    XERCES_CPP_NAMESPACE::DOMNodeList* stepSizeList = node->getElementsByTagName(
            XERCES_CPP_NAMESPACE::XMLString::transcode(stepSizeLabel));
    XERCES_CPP_NAMESPACE::DOMNodeList* coeffList = node->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode(stepSizeLabel));

    if ((startValueList->getLength() == 1) && (endValueList->getLength() == 1)){
        range->setStartValue( XMLaux::setValueToDouble(startValueList->item(0)) );
        range->setEndValue( XMLaux::setValueToDouble(endValueList->item(0)) );
        if (stepSizeList->getLength() == 1){
            range->setStepSize( XMLaux::setValueToDouble(stepSizeList->item(0)) );
        }
        if (coeffList->getLength() == 1){
			range->setCoeff( XMLaux::setValueToDouble(coeffList->item(0)) );
		}
    } else {
        error = true;
        std::stringstream out;
        out << "Error in parsing the input file: " << std::endl;
        out << "\tThe input has ranges badly defined." << std::endl;
        PrintingHandler::printOut(&out, PrintingHandler::STDERR);
    }

    return error;
}

int XMLaux::serializeDOM(XERCES_CPP_NAMESPACE::DOMNode* node, std::stringstream *output){

    using namespace xercesc;

    XMLCh tempStr[100];
    XMLString::transcode("LS", tempStr, 99);
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
    DOMLSSerializer* theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();

    // StdOutFormatTarget prints the resultant XML stream
    // to stdout once it receives any thing from the serializer.
    MemBufFormatTarget *myFormTarget = new MemBufFormatTarget();
    DOMLSOutput* theOutput = ((DOMImplementationLS*)impl)->createLSOutput();
    theOutput->setByteStream(myFormTarget);

    try {
        // do the serialization through DOMLSSerializer::write();
        theSerializer->write(node, theOutput);
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        std::cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (const DOMException& toCatch) {
        char* message = XMLString::transcode(toCatch.msg);
        std::cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (...) {
        std::cout << "Unexpected Exception \n" ;
        return -1;
    }

    // Transform to string
    const XMLByte* buffer = myFormTarget->getRawBuffer();
    unsigned int buffLength = myFormTarget->getLen();
    for ( unsigned int i = 0; i < buffLength; i++ ){
        *output << buffer[i];
    }

    theOutput->release();
    theSerializer->release();
    delete myFormTarget;
    return 0;
}

