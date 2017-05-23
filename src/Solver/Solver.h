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
 * File:   Solver.h
 * Author: s083781
 *
 * Created on March 16, 2011, 4:33 PM
 */

#ifndef SOLVER_H
#define	SOLVER_H

#include "Solution.h"
#include "../GeneralModel/GeneralModel.h"
#include "../Application/Input/Range.h"
#include "../Application/Input/TransientCurve.h"

//#include "gsl_odeiv_ext.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include <string>

namespace DYTSI_Solver{
    class Solver {
    public:
        Solver(DYTSI_Modelling::GeneralModel* gm, XERCES_CPP_NAMESPACE::DOMElement* el, int& error);
        virtual ~Solver();

        int solve(int index, double* y0, DYTSI_Input::Range *tspan, Solution& sol);
        int solveTransient(int index, double* y0, DYTSI_Input::Range *tspan, Solution& sol, DYTSI_Input::TransientCurve* transientCurve);
        int solveRamping(int index, double* y0, DYTSI_Input::Range *speedRange, Solution& sol);

        std::string getSolverInformation();

        virtual std::string getSolverName() const = 0;
        virtual const gsl_odeiv2_step_type* getStepType() = 0;
        virtual int setup() = 0;
        virtual int evolve(double* t, double tf, double* h, double* y) = 0;
        virtual int init() = 0;
        virtual std::string showInfo() = 0;
        virtual bool useDriver() = 0;
    protected:
        bool settedUp;

        double absPrecision;
        double relPrecision;
        double initStep;
        double storeFrequency;
        double jacStoreFrequency;

        DYTSI_Modelling::GeneralModel* model;
    };
}
#endif	/* SOLVER_H */

