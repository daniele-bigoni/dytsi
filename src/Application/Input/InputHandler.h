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
 * File:   InputList.h
 * Author: bigo
 *
 * Created on March 11, 2011, 1:25 PM
 */

#ifndef INPUTLIST_H
#define	INPUTLIST_H

#include "Input.h"
#include "XMLInputReader.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <vector>

namespace DYTSI_Input{

    class InputHandler{
    public:
        InputHandler();
        virtual ~InputHandler();

        std::vector<Input*> getInputList();
        char* getOutputFolder();

        bool parseFile(const char* path);
        bool elaborateParsedDocument(XERCES_CPP_NAMESPACE::DOMDocument* doc);

        void reset();

        bool createOutputFolder(const char* directory);
        
    private:
        XMLInputReader* inputReader;

        std::vector<Input*> inputList;

        char* outputFolder;


    };

}

#endif	/* INPUTLIST_H */

