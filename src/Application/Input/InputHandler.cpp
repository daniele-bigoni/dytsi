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
 * File:   InputList.cpp
 * Author: bigo
 * 
 * Created on March 11, 2011, 1:25 PM
 */

# include <boost/filesystem.hpp>

#include "InputHandler.h"
#include "Transient.h"
#include "Bifurcation.h"
#include "Ramping.h"

#include "../PrintingHandler.h"
#include "XMLaux.h"

#include <sstream>

namespace DYTSI_Input{
    InputHandler::InputHandler(){
        this->inputList.clear();
        this->inputReader = new XMLInputReader(this);
    }

    InputHandler::~InputHandler() {
    }

    std::vector<Input*> InputHandler::getInputList(){
        return this->inputList;
    }

    char* InputHandler::getOutputFolder(){
        return this->outputFolder;
    }

    void InputHandler::reset(){
        this->inputList.clear();
    }

    /**
     * Parse the file in path.
     *
     * @param path
     * @return
     */
    bool InputHandler::parseFile(const char* path){

        if (this->inputReader->parseFile(path) != 0){
            std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << "\tError in parsing the input file: " << path << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDERR);
            delete out;
            return true;
        }

        return false;
    }

    bool InputHandler::elaborateParsedDocument(XERCES_CPP_NAMESPACE::DOMDocument* doc){

        XERCES_CPP_NAMESPACE::DOMElement* root = (XERCES_CPP_NAMESPACE::DOMElement*)doc->getDocumentElement();

        // Reading and Creating the output Folder
        XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = root->getAttributes();
        XERCES_CPP_NAMESPACE::DOMAttr* nameAttribute =
                (XERCES_CPP_NAMESPACE::DOMAttr*)pAttributes->getNamedItem(
                XERCES_CPP_NAMESPACE::XMLString::transcode("OutputFolder"));
        this->outputFolder = XERCES_CPP_NAMESPACE::XMLString::transcode(nameAttribute->getValue());
        int errCode = this->createOutputFolder(this->outputFolder);
        if (errCode != 0) return errCode;

        XERCES_CPP_NAMESPACE::DOMNodeList* transientTests = root->getElementsByTagName(XERCES_CPP_NAMESPACE::XMLString::transcode("Transient"));
        XERCES_CPP_NAMESPACE::DOMNodeList* bifurcationTests = root->getElementsByTagName(XERCES_CPP_NAMESPACE::XMLString::transcode("Bifurcation"));
        XERCES_CPP_NAMESPACE::DOMNodeList* rampingTests = root->getElementsByTagName(XERCES_CPP_NAMESPACE::XMLString::transcode("Ramping"));

        for ( unsigned int i = 0; i < transientTests->getLength(); i++ ){
            int error = 0;
            XERCES_CPP_NAMESPACE::DOMElement* el = (XERCES_CPP_NAMESPACE::DOMElement*)transientTests->item(i);
            Input* transient = new Transient(this, el, error);
            if (error == 0)
                this->inputList.push_back(transient);
            else
                return true;
        }

        for ( unsigned int i = 0; i < bifurcationTests->getLength(); i++ ){
            int error = 0;
            XERCES_CPP_NAMESPACE::DOMElement* el = (XERCES_CPP_NAMESPACE::DOMElement*)bifurcationTests->item(i);
            Input* bifurcation = new Bifurcation(this, el, error);
            if (error == 0)
                this->inputList.push_back(bifurcation);
            else
                return true;
        }

        for ( unsigned int i = 0; i < rampingTests->getLength(); i++ ){
			int error = 0;
			XERCES_CPP_NAMESPACE::DOMElement* el = (XERCES_CPP_NAMESPACE::DOMElement*)rampingTests->item(i);
			Input* ramping = new Ramping(this, el, error);
			if (error == 0)
				this->inputList.push_back(ramping);
			else
				return true;
		}

        return false;
    }

    bool InputHandler::createOutputFolder(const char* directory){
        std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\tCreating output folder: " << directory << "\t" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;

        int errCode;
        if (boost::filesystem::exists(directory)){
        	out = new std::stringstream(std::stringstream::in|std::stringstream::out);
			*out << std::endl << "The folder " << directory << " already exists. Do you want to use it anyway? [y/n] ";
			PrintingHandler::printOut(out,PrintingHandler::STDOUT);
			delete out;

			char input = 'y';
			std::cin >> input;
			if (input != 'y'){
				out = new std::stringstream(std::stringstream::in|std::stringstream::out);
				*out << "Output folder initialization \t[FAILED]" << std::endl;
				PrintingHandler::printOut(out,PrintingHandler::STDOUT);
				delete out;
				errCode = 1;
			} else {
				out = new std::stringstream(std::stringstream::in|std::stringstream::out);
				*out << "Output folder initialization \t[DONE]" << std::endl;
				PrintingHandler::printOut(out,PrintingHandler::STDOUT);
				delete out;
				errCode = 0;
			}
        } else {
        	std::stringstream command;
        	command << "mkdir --parents \'" << directory << "\'";
        	errCode = system((command.str()).c_str());
        	if (errCode == 0){
				out = new std::stringstream(std::stringstream::in|std::stringstream::out);
				*out << "[DONE]" << std::endl;
				PrintingHandler::printOut(out,PrintingHandler::STDOUT);
				delete out;
			} else {
				out = new std::stringstream(std::stringstream::in|std::stringstream::out);
				*out << std::endl << "The folder " << directory << " could not be created." << std::endl;
				*out << "Output folder initialization \t[FAILED]" << std::endl;
				PrintingHandler::printOut(out,PrintingHandler::STDOUT);
				delete out;
			}
        }

        return errCode;
    }
}
