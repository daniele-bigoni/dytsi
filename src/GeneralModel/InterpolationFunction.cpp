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
 * File:   InterpolationFunction.cpp
 * Author: bigo
 * 
 * Created on July 3, 2011, 11:23 AM
 */

#include "InterpolationFunction.h"

namespace DYTSI_Modelling{
    InterpolationFunction::InterpolationFunction(InterpolationTypes interpType) {
        this->interpType = interpType;
    }

    InterpolationFunction::~InterpolationFunction() {
    }

    gsl_interp* InterpolationFunction::allocInterp(int size){
        switch (this->interpType){
            case INTP_LINEAR:
                return gsl_interp_alloc (gsl_interp_linear, size);
            case INTP_CUBICSPLINE:
                return gsl_interp_alloc (gsl_interp_cspline, size);
            case INTP_AKIMA:
                return gsl_interp_alloc (gsl_interp_akima, size);
            default:
                return gsl_interp_alloc (gsl_interp_linear, size);
        }
    }
}

