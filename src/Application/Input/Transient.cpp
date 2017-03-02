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
 * File:   Transient.cpp
 * Author: bigo
 * 
 * Created on March 11, 2011, 1:46 PM
 */

#include "Transient.h"

#include "XMLaux.h"
#include "../PrintingHandler.h"

#include "../../GeneralModel/ExternalComponents.h"

#include <cstdlib>
#include <sstream>
#include <vector>
#include <iosfwd>

namespace DYTSI_Input{
    Transient::Transient( DYTSI_Input::InputHandler* inputHandler, XERCES_CPP_NAMESPACE::DOMElement* el, int& error)
    : Input( inputHandler, el, error ) {
        std::stringstream* out;
        this->y0.clear();
        this->tSpan = new Range();

        XERCES_CPP_NAMESPACE::DOMNodeList* tSpanRangeList = el->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode("TSpan"));
        XERCES_CPP_NAMESPACE::DOMNodeList* speedList = el->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode("Speed"));
        XERCES_CPP_NAMESPACE::DOMNodeList* radiusList = el->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode("Radius"));
        XERCES_CPP_NAMESPACE::DOMNodeList* Phi_seList = el->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode("Phi_se"));
        XERCES_CPP_NAMESPACE::DOMNodeList* startingValuesList = el->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode("StartingValues"));

        if ((speedList->getLength() == 1) && (radiusList->getLength() == 1) &&
                (Phi_seList->getLength() == 1) && (startingValuesList->getLength() == 1) &&
                (tSpanRangeList->getLength() == 1)){
            error += XMLaux::setRange(
                    (XERCES_CPP_NAMESPACE::DOMElement*)tSpanRangeList->item(0), "StartValue", "EndValue", "", this->tSpan);
            this->speed = XMLaux::setValueToDouble(speedList->item(0));
            this->radius = XMLaux::setValueToDouble(radiusList->item(0));
            this->Phi_se = XMLaux::setValueToDouble(Phi_seList->item(0));

            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << "\t\tLoading Starting Values. \t" << std::flush;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            delete out;
            error += this->getModel()->setStartingValues((XERCES_CPP_NAMESPACE::DOMElement*)startingValuesList->item(0), this->y0);
            if (error > 0){
                out = new std::stringstream(std::stringstream::in|std::stringstream::out);
                *out << "[FAILED]" << std::endl;
                PrintingHandler::printOut(out, PrintingHandler::STDOUT);
                delete out;
            } else {
                out = new std::stringstream(std::stringstream::in|std::stringstream::out);
                *out << "[DONE]" << std::endl;
                PrintingHandler::printOut(out, PrintingHandler::STDOUT);
                delete out;

//                // Debug
//                out = new std::stringstream(std::stringstream::in|std::stringstream::out);
//                *out << "Starting values: size " << this->y0.size() << std::endl;
//                for (std::vector<double>::iterator it = this->y0.begin(); it != this->y0.end(); ++it){
//                    *out << *it << " ";
//                }
//                *out << std::endl;
//                OutputHandler::printOut(out, OutputHandler::STDOUT);
//                delete out;
            }
        } else {
            error = 1;
            std::stringstream out;
            out << "Error in parsing the input file: " << std::endl;
            out << "\t " << this->getTestName() << ": The Transient input has not the proper number of arguments." << std::endl;
            out << "\t \t Speed Size: \t" << speedList->getLength() << std::endl;
            out << "\t \t Radius Size: \t" << radiusList->getLength() << std::endl;
            out << "\t \t Phi_se Size: \t" << Phi_seList->getLength() << std::endl;
            out << "\t \t StartingValue Size: \t" << startingValuesList->getLength() << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDERR);
        }
        
    }

    Transient::~Transient() {
    }

    void Transient::setStartingValues(XERCES_CPP_NAMESPACE::DOMNode* node){
        char* nodeValue = XERCES_CPP_NAMESPACE::XMLString::transcode(node->getFirstChild()->getNodeValue());
        std::stringstream ss (nodeValue,std::stringstream::in | std::stringstream::out);
        while (ss.rdstate() != std::stringstream::eofbit){
            double val;
            ss >> val;
            this->y0.push_back(val);
        }
    }

    DYTSI_Solver::Solution* Transient::simulate(){
        int indexSimulation = 0;
        int errCode = GSL_SUCCESS;

        std::stringstream* out;

        // Create a new solution structure
        DYTSI_Solver::Solution* sol = new DYTSI_Solver::Solution(this->getModel(), this->getOutputHandler());

        // Cover the transient curve before starting the simulation
        if (this->transientCurve->isSettedCant() || this->transientCurve->isSettedRadius()){
            errCode = this->coverTransientCurve(indexSimulation,*sol,
                    this->speed, this->radius, this->Phi_se);
            indexSimulation++;
        }

        if (errCode == GSL_SUCCESS) {
            // Starting the Transient analysis
            out = new std::stringstream(std::stringstream::in | std::stringstream::out);
            *out << std::endl << "#####################################" << std::endl;
            *out << "\t Transient Analysis: " << this->getTestName() << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            delete out;

            // Set up the external components
            this->getModel()->getExternalComponents()->set_v(this->speed);
            this->getModel()->getExternalComponents()->set_R(this->radius);
            this->getModel()->getExternalComponents()->set_Phi_se(this->Phi_se);

            double* yInit;
            if ( sol->getSolutionList().size() > 0 ){
                // Get the last status
                yInit = (sol->getStatusList())[ sol->getStatusList().size() -1 ];
            } else {
                // Init with the starting conditions
                yInit = new double[this->getModel()->get_N_DOF()];
                for (int i = 0; i < this->getModel()->get_N_DOF(); i++) yInit[i] = this->y0[i];
            }

            // Solve the system and get the timing
            errCode = this->getSolver()->solve(indexSimulation, yInit, this->tSpan, *sol);

            // Ending the transient analysis
            out = new std::stringstream(std::stringstream::in | std::stringstream::out);
            *out << std::endl << "\t Transient Analysis: " << this->getTestName() << "\t[DONE]"<< std::endl;
            *out << "#####################################" << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            delete out;
        }

        return sol;
    }
}
