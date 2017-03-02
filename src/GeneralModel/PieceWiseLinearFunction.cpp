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
 * File:   NonLinearFunction.cpp
 * Author: bigo
 * 
 * Created on May 1, 2011, 2:54 PM
 */

#include "PieceWiseLinearFunction.h"

#include <gsl/gsl_interp.h>
#include <gsl/gsl_errno.h>

namespace DYTSI_Modelling {
    PieceWiseLinearFunction::PieceWiseLinearFunction(PieceWiseValues* PWLVals)
    : InterpolationFunction(PWLVals->getInterpType())
    {
        this->PWLVals = PWLVals;
        this->acc = gsl_interp_accel_alloc();
        this->interp = this->allocInterp(PWLVals->getSize());
        double** vals = PWLVals->getVals();
        gsl_interp_init( this->interp, vals[0], vals[1], PWLVals->getSize());
    }

    PieceWiseLinearFunction::~PieceWiseLinearFunction() {
        gsl_interp_free(this->interp);
        gsl_interp_accel_free(this->acc);
    }

    int PieceWiseLinearFunction::func(double val, double &res) {
        int errCode;

        double** vals = PWLVals->getVals();
        double* xs = vals[0];
        double* ys = vals[1];
        int size = PWLVals->getSize();
        if (( val >= xs[0] ) && ( val <= xs[size-1] )){
            res = gsl_interp_eval( this->interp, xs, ys, val, this->acc );
            errCode = GSL_SUCCESS;
        } else {
            errCode = GSL_EDOM;
        }

        return errCode;
    }

    FunctionTypes PieceWiseLinearFunction::getType(){
        return FT_PIECEWISELINEAR;
    }
}

