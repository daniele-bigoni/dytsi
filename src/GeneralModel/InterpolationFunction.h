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
 * File:   InterpolationFunction.h
 * Author: bigo
 *
 * Created on July 3, 2011, 11:23 AM
 */

#ifndef INTERPOLATIONFUNCTION_H
#define	INTERPOLATIONFUNCTION_H

#include "enumDeclarations.h"

#include <gsl/gsl_interp.h>
#include <gsl/gsl_errno.h>

namespace DYTSI_Modelling{
    class InterpolationFunction {
    public:
        InterpolationFunction(InterpolationTypes interpType);
        virtual ~InterpolationFunction();
        
    private:
        InterpolationTypes interpType;

    protected:
        gsl_interp* allocInterp(int size);
    };
}

#endif	/* INTERPOLATIONFUNCTION_H */

