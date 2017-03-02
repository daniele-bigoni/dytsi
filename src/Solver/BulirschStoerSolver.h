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
 * File:   BulirschStoerSolver.h
 * Author: s083781
 *
 * Created on March 16, 2011, 5:35 PM
 */

#ifndef BULIRSCHSTOERSOLVER_H
#define	BULIRSCHSTOERSOLVER_H

#include "Solver.h"

namespace DYTSI_Solver {
    class BulirschStoerSolver : public Solver{
    public:
        BulirschStoerSolver(DYTSI_Modelling::GeneralModel* gm, XERCES_CPP_NAMESPACE::DOMElement* el, int& error);
        virtual ~BulirschStoerSolver();

        int setup();
        int evolve(double* t, double tf, double* h, double* y);
        int init();
        std::string showInfo();
        bool useDriver();

        std::string getSolverName() const;
        const gsl_odeiv2_step_type* getStepType();
    private:
        gsl_odeiv2_step * s;
        gsl_odeiv2_control * c;
        gsl_odeiv2_evolve * e;
    };
}

#endif	/* BULIRSCHSTOERSOLVER_H */

