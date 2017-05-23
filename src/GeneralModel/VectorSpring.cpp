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
 * File:   LinearLinkSpring.cpp
 * Author: bigo
 * 
 * Created on March 29, 2011, 9:06 AM
 */

#include "VectorSpring.h"
#include "Function.h"

#include <gsl/gsl_blas.h>

namespace DYTSI_Modelling {
    VectorSpring::VectorSpring(std::string name, gsl_vector* connectionPointUp,
            gsl_vector* connectionPointDown, Function* stiffnessFunction)
    : LinkSpring(name, connectionPointUp, connectionPointDown) {
        this->stiffnessFunction = stiffnessFunction;
        this->u = gsl_vector_alloc(3);
    }

    VectorSpring::~VectorSpring() {
    }

    int VectorSpring::force( RelativeComponentPosition posConnector,
                gsl_vector* lengthVector, gsl_vector* F ){
        int errCode = GSL_SUCCESS;

        // Find the Euclidean length
        double euclideanLength = gsl_blas_dnrm2(lengthVector);

        // Find the Euclidean length at rest
        double lengthAtRest = gsl_blas_dnrm2(this->getVecLowerToUpperAtRest());

        // Find the deformation of the spring
        double dl = euclideanLength - lengthAtRest;

        // Compute the force function (stretching is positive) -> the force is positive
        // but will be projected on the length direction
        double Fmag;
        errCode = this->stiffnessFunction->func(dl, Fmag);
        if ( errCode != GSL_SUCCESS ){
            return errCode;
        }

        // Compute the direction vector (unit vector)
        gsl_vector_memcpy(u, lengthVector);
        gsl_blas_dscal( (1.0/euclideanLength), u );

        // Compute the Force vector for this spring
        gsl_vector_memcpy( F, u );
        gsl_blas_dscal( Fmag, F );

        return errCode;
    }

    Function* VectorSpring::getStiffnessFunctionZ(){
        return this->stiffnessFunction;
    }
}
