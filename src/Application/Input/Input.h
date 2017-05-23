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
 * File:   Input.h
 * Author: bigo
 *
 * Created on March 11, 2011, 1:29 PM
 */

#ifndef INPUT_H
#define	INPUT_H

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include "../../GeneralModel/GeneralModel.h"
#include "../../Solver/Solver.h"
#include "TransientCurve.h"

namespace DYTSI_Output{
    class OutputHandler;
}

namespace DYTSI_Input{
    class InputHandler;

    class Input {
    public:
        Input( DYTSI_Input::InputHandler* inputHandler, XERCES_CPP_NAMESPACE::DOMElement* node, int& error);
        virtual ~Input();

        const char* getTestName(){ return testName; }
        DYTSI_Modelling::GeneralModel* getModel();
        DYTSI_Input::TransientCurve* getTransientCurve();
        DYTSI_Input::InputHandler* getInputHandler();
        DYTSI_Solver::Solver* getSolver();
        DYTSI_Output::OutputHandler* getOutputHandler();

        virtual DYTSI_Solver::Solution* simulate() = 0;

        int coverTransientCurve( int index, DYTSI_Solver::Solution &sol,
            double speedSim, double radiusSim, double Phi_seSim);
        
    protected:
        char* testName;
        DYTSI_Modelling::GeneralModel* model;
        DYTSI_Input::TransientCurve* transientCurve;
        DYTSI_Solver::Solver* solver;
        DYTSI_Output::OutputHandler* outputHandler;
        DYTSI_Input::InputHandler* inputHandler;
        std::vector<double> y0;
    };
}

#endif	/* INPUT_H */

