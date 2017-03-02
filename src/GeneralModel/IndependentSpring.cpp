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
 * File:   IndependentSpring.cpp
 * Author: s083781
 * 
 * Created on April 7, 2011, 3:57 PM
 */

#include "IndependentSpring.h"
#include "Link.h"

#include <gsl/gsl_blas.h>

namespace DYTSI_Modelling {
    IndependentSpring::IndependentSpring(std::string name, gsl_vector* connectionPointUp, gsl_vector* connectionPointDown,
                Function* stiffnessFunctionX, Function* stiffnessFunctionY, Function* stiffnessFunctionZ)
    : LinkSpring(name, connectionPointUp, connectionPointDown){
        this->stiffnessFunctionX = stiffnessFunctionX;
        this->stiffnessFunctionY = stiffnessFunctionY;
        this->stiffnessFunctionZ = stiffnessFunctionZ;
    }

    IndependentSpring::~IndependentSpring() {
    }

    int IndependentSpring::force(RelativeComponentPosition posConnector,
            gsl_vector* lengthVector, gsl_vector* F){
        int errCode = GSL_SUCCESS;

        gsl_vector* dl = gsl_vector_alloc(3);

        // Revert the vector if necessary
        double oper;
        if (posConnector == UP) {
            oper = -1.0;
        } else {
            oper = 1.0;
        }

        // Compute the displacment component by component
        gsl_vector_memcpy(dl, lengthVector);
        gsl_blas_daxpy(oper, this->getVecLowerToUpperAtRest(), dl);

        // Compute the magnitude of the force
        double FmagX;
        errCode = this->stiffnessFunctionX->func(gsl_vector_get(dl,0), FmagX);
        if (errCode != GSL_SUCCESS){
            gsl_vector_free(dl);
            return errCode;
        }
        double FmagY;
        errCode = this->stiffnessFunctionY->func(gsl_vector_get(dl,1),FmagY);
        if (errCode != GSL_SUCCESS){
            gsl_vector_free(dl);
            return errCode;
        }
        double FmagZ;
        errCode = this->stiffnessFunctionZ->func(gsl_vector_get(dl,2), FmagZ);
        if (errCode != GSL_SUCCESS){
            gsl_vector_free(dl);
            return errCode;
        }

        // Compute the Force vector
        gsl_vector_set(F,0,FmagX);
        gsl_vector_set(F,1,FmagY);
        gsl_vector_set(F,2,FmagZ);

        // Free memory
        gsl_vector_free(dl);

        return errCode;
    }

    Function* IndependentSpring::getStiffnessFunctionZ(){
        return this->stiffnessFunctionZ;
    }
}
