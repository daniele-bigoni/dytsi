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
 * File:   XMLInputReader.cpp
 * Author: bigo
 * 
 * Created on March 11, 2011, 12:40 PM
 */

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>

#include <string.h>
#include <stdlib.h>

#include "XMLInputReader.h"
#include "InputHandler.h"

#if defined(XERCES_NEW_IOSTREAMS)
#include <fstream>
#else
#include <fstream.h>
#endif

DOMCountErrorHandler::DOMCountErrorHandler() :

    fSawErrors(false)
{
}

DOMCountErrorHandler::~DOMCountErrorHandler()
{
}


// ---------------------------------------------------------------------------
//  DOMCountHandlers: Overrides of the DOM ErrorHandler interface
// ---------------------------------------------------------------------------
bool DOMCountErrorHandler::handleError(const XERCES_CPP_NAMESPACE::DOMError& domError)
{
    fSawErrors = true;
    if (domError.getSeverity() == XERCES_CPP_NAMESPACE::DOMError::DOM_SEVERITY_WARNING)
        XERCES_STD_QUALIFIER cerr << "\nWarning at file ";
    else if (domError.getSeverity() == XERCES_CPP_NAMESPACE::DOMError::DOM_SEVERITY_ERROR)
        XERCES_STD_QUALIFIER cerr << "\nError at file ";
    else
        XERCES_STD_QUALIFIER cerr << "\nFatal Error at file ";

    XERCES_STD_QUALIFIER cerr << StrX(domError.getLocation()->getURI())
         << ", line " << domError.getLocation()->getLineNumber()
         << ", char " << domError.getLocation()->getColumnNumber()
         << "\n  Message: " << StrX(domError.getMessage()) << XERCES_STD_QUALIFIER endl;

    return true;
}

void DOMCountErrorHandler::resetErrors()
{
    fSawErrors = false;
}

namespace DYTSI_Input {
    XMLInputReader::XMLInputReader(DYTSI_Input::InputHandler* inputHandler) {
        this->inputHandler = inputHandler;
    }

    XMLInputReader::~XMLInputReader() {
    }

    bool XMLInputReader::parseFile(const char* inputFile){
        const char*                xmlFile = 0;
        XERCES_CPP_NAMESPACE::AbstractDOMParser::ValSchemes valScheme = XERCES_CPP_NAMESPACE::AbstractDOMParser::Val_Auto;
        bool                       doNamespaces       = false;
        bool                       doSchema           = true;
        bool                       schemaFullChecking = true;
        bool                       doList = false;
        bool                       errorOccurred = false;
        bool                       recognizeNEL = false;
        char*                      localeStr = "en_US";

        // Initialize the XML4C system
        try
        {
            if (strlen(localeStr))
            {
                XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize(localeStr);
            }
            else
            {
                XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
            }

            if (recognizeNEL)
            {
                XERCES_CPP_NAMESPACE::XMLPlatformUtils::recognizeNEL(recognizeNEL);
            }
        }

        catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch)
        {
             XERCES_STD_QUALIFIER cerr << "Error during initialization! :\n"
                  << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
             return 1;
        }

        // Instantiate the DOM parser.
        static const XMLCh gLS[] = { XERCES_CPP_NAMESPACE::chLatin_L, XERCES_CPP_NAMESPACE::chLatin_S, XERCES_CPP_NAMESPACE::chNull };
        XERCES_CPP_NAMESPACE::DOMImplementation *impl = XERCES_CPP_NAMESPACE::DOMImplementationRegistry::getDOMImplementation(gLS);
        XERCES_CPP_NAMESPACE::DOMLSParser       *parser = ((XERCES_CPP_NAMESPACE::DOMImplementationLS*)impl)->createLSParser(XERCES_CPP_NAMESPACE::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
        XERCES_CPP_NAMESPACE::DOMConfiguration  *config = parser->getDomConfig();

        config->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMNamespaces, doNamespaces);
        config->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgXercesSchema, doSchema);
        config->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgXercesHandleMultipleImports, true);
        config->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);

        if (valScheme == XERCES_CPP_NAMESPACE::AbstractDOMParser::Val_Auto)
        {
            config->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMValidateIfSchema, true);
        }
        else if (valScheme == XERCES_CPP_NAMESPACE::AbstractDOMParser::Val_Never)
        {
            config->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMValidate, false);
        }
        else if (valScheme == XERCES_CPP_NAMESPACE::AbstractDOMParser::Val_Always)
        {
            config->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMValidate, true);
        }

        // enable datatype normalization - default is off
        config->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMDatatypeNormalization, true);

        // And create our error handler and install it
        DOMCountErrorHandler errorHandler;
        config->setParameter(XERCES_CPP_NAMESPACE::XMLUni::fgDOMErrorHandler, &errorHandler);


        //
        //  Get the starting time and kick off the parse of the indicated
        //  file. Catch any exceptions that might propogate out of it.
        //
        unsigned long duration;

        bool more = true;
        XERCES_STD_QUALIFIER ifstream fin;

        // the input is a list file
        if (doList)
            fin.open(inputFile);

        if (fin.fail()) {
            XERCES_STD_QUALIFIER cerr <<"Cannot open the list file: " << inputFile << XERCES_STD_QUALIFIER endl;
            return 2;
        }

        while (more)
        {
            char fURI[1000];
            //initialize the array to zeros
            memset(fURI,0,sizeof(fURI));

            if (doList) {
                if (! fin.eof() ) {
                    fin.getline (fURI, sizeof(fURI));
                    if (!*fURI)
                        continue;
                    else {
                        xmlFile = fURI;
                        XERCES_STD_QUALIFIER cerr << "==Parsing== " << xmlFile << XERCES_STD_QUALIFIER endl;
                    }
                }
                else
                    break;
            }
            else {
                xmlFile = inputFile;
                more = false;
            }

            //reset error count first
            errorHandler.resetErrors();

            XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;
            try
            {
                // reset document pool
                parser->resetDocumentPool();

                const unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
                doc = parser->parseURI(xmlFile);
                const unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
                duration = endMillis - startMillis;
            }

            catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch)
            {
                XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << xmlFile << "'\n"
                     << "Exception message is:  \n"
                     << StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
                errorOccurred = true;
                continue;
            }
            catch (const XERCES_CPP_NAMESPACE::DOMException& toCatch)
            {
                const unsigned int maxChars = 2047;
                XMLCh errText[maxChars + 1];

                XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << xmlFile << "'\n"
                     << "DOMException code is:  " << toCatch.code << XERCES_STD_QUALIFIER endl;

                if (XERCES_CPP_NAMESPACE::DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
                     XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;

                errorOccurred = true;
                continue;
            }
            catch (...)
            {
                XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << xmlFile << "'\n";
                errorOccurred = true;
                continue;
            }

            //
            //  Extract the DOM tree, get the list of all the elements and report the
            //  length as the count of elements.
            //
            if (errorHandler.getSawErrors())
            {
                XERCES_STD_QUALIFIER cout << "\nErrors occurred, no output available\n" << XERCES_STD_QUALIFIER endl;
                errorOccurred = true;
            }
            else {
                bool error = this->inputHandler->elaborateParsedDocument(doc);
                if (error) return error;
            }
        }

        //
        //  Delete the parser itself.  Must be done prior to calling Terminate, below.
        //
        parser->release();

        // And call the termination method
        XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();

        if (doList)
            fin.close();

        if (errorOccurred)
            return 4;
        else
            return 0;
    }
}
