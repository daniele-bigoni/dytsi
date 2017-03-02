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
 * File:   LinearDamper.cpp
 * Author: s083781
 * 
 * Created on February 11, 2011, 4:17 PM
 */

#include "Function.h"


#include "VectorDamper.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

namespace DYTSI_Modelling {
    VectorDamper::VectorDamper(std::string name, gsl_vector* connectionPointUp,
                gsl_vector* connectionPointDown, Function* dampingFunction)
    : LinkDamper( name, connectionPointUp, connectionPointDown) {
        this->dampingFunction = dampingFunction;
        this->u = gsl_vector_alloc(3);
    }

    VectorDamper::~VectorDamper() {
    }

    int VectorDamper::force (RelativeComponentPosition posConnector,  gsl_vector* speed, gsl_vector* lengthVector, gsl_vector* F){

        int errCode = GSL_SUCCESS;

        // Find the Euclidean length of the length vector
        double lengthMagnitude = gsl_blas_dnrm2(lengthVector);

        ////////
        // Project the velocity vector on the working direction of the damper 
        double dotProdRes;
        gsl_blas_ddot(speed, lengthVector, &dotProdRes);
        double velocityMagnitudeN = dotProdRes / lengthMagnitude;
        ////////

        // Compute the resulting force magnitude
        double Fmag;
        errCode = this->dampingFunction->func(velocityMagnitudeN, Fmag);
        if (errCode != GSL_SUCCESS)
            return errCode;

        // Compute the direction vector (unit vector) of the speed projected on the
        // damper's direction
        if (lengthMagnitude > 0.0){
            gsl_vector_memcpy(u, lengthVector);
            gsl_blas_dscal( (1.0/lengthMagnitude), u );
        } else {
            gsl_vector_set_zero(u);
        }

        // Compute the Force vector for this damper
        gsl_vector_memcpy( F, u );
        gsl_blas_dscal( Fmag, F );

        return errCode;
    }
}
