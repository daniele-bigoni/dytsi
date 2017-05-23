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
 * File:   PieceWiseLinearFunction.h
 * Author: bigo
 *
 * Created on May 1, 2011, 2:54 PM
 */

#ifndef PIECEWISELINEARFUNCTION_H
#define	PIECEWISELINEARFUNCTION_H

#include "Function.h"
#include "PieceWiseValues.h"
#include "InterpolationFunction.h"

#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

namespace DYTSI_Modelling {
    class PieceWiseLinearFunction : public Function, public InterpolationFunction {
    public:
        PieceWiseLinearFunction(PieceWiseValues* PWLVals);
        virtual ~PieceWiseLinearFunction();

        int func(double val, double &res);
        FunctionTypes getType();
    private:
        InterpolationTypes interpType;
        PieceWiseValues* PWLVals;
        gsl_interp_accel* acc;
        gsl_interp* interp;

    };
}

#endif	/* PIECEWISELINEARFUNCTION_H */

