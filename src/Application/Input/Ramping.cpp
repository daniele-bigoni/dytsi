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
 * Ramping.cpp
 *
 *  Created on: Dec 5, 2012
 *      Author: Daniele Bigoni (dabi@imm.dtu.dk)
 */

#include "Ramping.h"

#include "XMLaux.h"
#include "../PrintingHandler.h"

#include "../../GeneralModel/ExternalComponents.h"

#include <cstdlib>
#include <sstream>
#include <vector>

namespace DYTSI_Input {
	Ramping::Ramping( DYTSI_Input::InputHandler* inputHandler, XERCES_CPP_NAMESPACE::DOMElement* node, int& error)
    : Input( inputHandler, node, error ) {
        std::stringstream* out;

        // Init variables
        this->y0.clear();
        this->speedRange = new Range();
        this->radiusRange = new Range();
        this->Phi_seRange = new Range();

        // Load data from XML
        XERCES_CPP_NAMESPACE::DOMNodeList* speedRangeList = node->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode("SpeedRange"));
        XERCES_CPP_NAMESPACE::DOMNodeList* radiusRangeList = node->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode("RadiusRange"));
        XERCES_CPP_NAMESPACE::DOMNodeList* Phi_seRangeList = node->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode("Phi_seRange"));
        XERCES_CPP_NAMESPACE::DOMNodeList* startingValuesList = node->getElementsByTagName(
                XERCES_CPP_NAMESPACE::XMLString::transcode("StartingValues"));

        if ((speedRangeList->getLength() == 1) && (radiusRangeList->getLength() == 1) &&
                (Phi_seRangeList->getLength() == 1) && (startingValuesList->getLength() == 1)){

        	// Set up Speed range, Radius and Cant
            error += XMLaux::setRange(
                    (XERCES_CPP_NAMESPACE::DOMElement*)speedRangeList->item(0),
                    "StartValue", "EndValue", "AccelCoeff", this->speedRange);
            error += XMLaux::setRange(
                    (XERCES_CPP_NAMESPACE::DOMElement*)radiusRangeList->item(0),
                    "StartValue", "EndValue", "StepSize", this->radiusRange);
            error += XMLaux::setRange(
                    (XERCES_CPP_NAMESPACE::DOMElement*)Phi_seRangeList->item(0),
                    "StartValue", "EndValue", "StepSize", this->Phi_seRange);

            // Set up starting values
            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << "\t\tLoading Starting Values. \t" << std::flush;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            delete out;
            XERCES_CPP_NAMESPACE::DOMElement* startingValuesEl = (XERCES_CPP_NAMESPACE::DOMElement*)startingValuesList->item(0);
            // Load the starting values
            error += this->getModel()->setStartingValues(startingValuesEl, this->y0);
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
            }

//            // Set up system for additional DOF for acceleration
//            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
//            *out << "\t\tSet up model acceleration. \t" << std::flush;
//            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
//			delete out;
//			error += this->getModel()->setAcceleration();
//			if (error > 0){
//				out = new std::stringstream(std::stringstream::in|std::stringstream::out);
//				*out << "[FAILED]" << std::endl;
//				PrintingHandler::printOut(out, PrintingHandler::STDOUT);
//				delete out;
//			} else {
//				out = new std::stringstream(std::stringstream::in|std::stringstream::out);
//				*out << "[DONE]" << std::endl;
//				PrintingHandler::printOut(out, PrintingHandler::STDOUT);
//				delete out;
//			}
        } else {
            error = 1;
            std::stringstream out;
            out << "Error in parsing the input file: " << std::endl;
            out << "\t " << this->getTestName() << ": The Ramping input has not the proper number of arguments." << std::endl;
            out << "\t \t Speed Size: \t" << speedRangeList->getLength() << std::endl;
            out << "\t \t Radius Size: \t" << radiusRangeList->getLength() << std::endl;
            out << "\t \t Phi_se Size: \t" << Phi_seRangeList->getLength() << std::endl;
            out << "\t \t StartingValue Size: \t" << startingValuesList->getLength() << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDERR);
        }
    }

    Ramping::~Ramping() {
    }

    DYTSI_Solver::Solution* Ramping::simulate(){
        int indexSimulation = 0;
        int errCode = GSL_SUCCESS;

        std::stringstream* out;

        // Create a new solution structure
        DYTSI_Solver::Solution* sol = new DYTSI_Solver::Solution(this->getModel(), this->getOutputHandler());

        // Cover the transient curve before starting the simulation
        if (this->transientCurve->isSettedCant() || this->transientCurve->isSettedRadius()){
            errCode = this->coverTransientCurve(indexSimulation, *sol, this->speedRange->getStartValue(),
                    this->radiusRange->getStartValue(), this->Phi_seRange->getStartValue());
            indexSimulation++;
        }

        if (errCode == GSL_SUCCESS){
            // Starting the Ramping analysis
            out = new std::stringstream(std::stringstream::in | std::stringstream::out);
            *out << std::endl << "#####################################" << std::endl;
            *out << "\t Bifurcation Analysis: " << this->getTestName() << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            delete out;

            double speed;
            double endSpeed;
            double radius;
            double endRadius;
            double phi_se;
            double endPhi_se;

            // bif... is -1 if the values goes downward, 0 if they don't change, 1 if they go upward
            int bifSpeed;
            int bifRadius;
            int bifPhi_se;

            // Exit function for bif.. = 0
            bool exitSpeed;
            bool exitRadius;
            bool exitPhi_se;

            speed = this->speedRange->getStartValue();
            endSpeed = this->speedRange->getEndValue();
            radius = this->radiusRange->getStartValue();
            endRadius = this->radiusRange->getEndValue();
            phi_se = this->Phi_seRange->getStartValue();
            endPhi_se = this->Phi_seRange->getEndValue();

            if (radius == endRadius) bifRadius = 0;
            if (radius > endRadius) bifRadius = -1;
            if (radius < endRadius) bifRadius = 1;

            if (phi_se == endPhi_se) bifPhi_se = 0;
            if (phi_se > endPhi_se) bifPhi_se = -1;
            if (phi_se < endPhi_se) bifPhi_se = 1;

            this->getModel()->getExternalComponents()->set_v( speed );
            this->getModel()->getExternalComponents()->set_R( radius );
            this->getModel()->getExternalComponents()->set_Phi_se( phi_se );

            double* yInit;
            if ( sol->getSolutionList().size() > 0 ){
				// Get the last status
				yInit = (sol->getStatusList())[ sol->getStatusList().size() -1 ];
				sol->addEntry(indexSimulation, 0.0, yInit, 0.0);
			} else {
				// Load the starting values
				yInit = new double[this->getModel()->get_N_DOF()];
				for (int i = 0; i < this->getModel()->get_N_DOF(); i++) yInit[i] = this->y0[i];
				sol->addEntry(indexSimulation, 0.0, yInit, 0.0);
			}

			radius = this->radiusRange->getStartValue();
			exitRadius = false;
			while ((    ((radius <= this->radiusRange->getEndValue()) && (bifRadius == 1)) ||
						((radius >= this->radiusRange->getEndValue()) && (bifRadius == -1))  || (bifRadius == 0))&&
						(errCode == GSL_SUCCESS)) {

				phi_se = this->Phi_seRange->getStartValue();
				exitPhi_se = false;
				while ((    ((phi_se <= this->Phi_seRange->getEndValue()) && (bifPhi_se == 1)) ||
							((phi_se >= this->Phi_seRange->getEndValue()) && (bifPhi_se == -1)) || (bifPhi_se == 0)) &&
						(errCode == GSL_SUCCESS)) {

					this->getModel()->getExternalComponents()->set_R( radius );
					this->getModel()->getExternalComponents()->set_Phi_se( phi_se );

					yInit = (sol->getStatusList())[ sol->getStatusList().size() -1 ];

					errCode = this->getSolver()->solveRamping(indexSimulation, yInit, this->speedRange, *sol);
					indexSimulation++;

					if (bifPhi_se == 0)
						break;
					else
						phi_se += this->Phi_seRange->getStepSize();

				}

				if (bifRadius == 0)
					break;
				else
					radius += this->radiusRange->getStepSize();
			}

        }

        // Ending the Ramping analysis
        out = new std::stringstream(std::stringstream::in | std::stringstream::out);
        *out << std::endl << "\t Ramping Analysis: " << this->getTestName() << "\t[DONE]"<< std::endl;
        *out << "#####################################" << std::endl;
        PrintingHandler::printOut(out, PrintingHandler::STDOUT);
        delete out;

        return sol;
    }
}
