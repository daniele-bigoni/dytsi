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
 * File:   SDIRKSolver.h
 * Author: bigo
 *
 * Created on May 8, 2011, 5:05 PM
 */

#ifndef SDIRKSOLVER_H
#define	SDIRKSOLVER_H

#include "Solver.h"

#include "SDIRK/sdirk.h"

namespace DYTSI_Solver {
    class SDIRKSolver : public Solver {
    public:
        SDIRKSolver(DYTSI_Modelling::GeneralModel* gm, XERCES_CPP_NAMESPACE::DOMElement* el, int& error, SDIRKMethodType type);
        virtual ~SDIRKSolver();

        int setup();
        int evolve(double* t, double tf, double* h, double* y);
        int init();
        std::string showInfo();
        bool useDriver();

        std::string getSolverName() const;
        const gsl_odeiv2_step_type* getStepType();

    private:
        Sdirk *SDIRKInstance;
        SDIRKMethodType methodType;

        bool isFirstStep;

    };
}

#endif	/* SDIRKSOLVER_H */

