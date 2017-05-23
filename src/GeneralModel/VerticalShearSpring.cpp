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
 * File:   ShearSpring.cpp
 * Author: s083781
 * 
 * Created on May 11, 2011, 4:20 PM
 */

#include "VerticalShearSpring.h"

#include <gsl/gsl_blas.h>

#include <cmath>

namespace DYTSI_Modelling {
    VerticalShearSpring::VerticalShearSpring(std::string name, gsl_vector* connectionPointUp, gsl_vector* connectionPointDown,
                Function* stiffnessFunctionZ, Function* stiffnessFunctionXY)
    : LinkSpring(name, connectionPointUp, connectionPointDown){
        this->stiffnessFunctionZ = stiffnessFunctionZ;
        this->stiffnessFunctionXY = stiffnessFunctionXY;
    }

    VerticalShearSpring::~VerticalShearSpring() {
    }

    int VerticalShearSpring::force(RelativeComponentPosition posConnector,
            gsl_vector* lengthVector, gsl_vector* F){
        int errCode = GSL_SUCCESS;

        gsl_vector_set_zero(F);

        // VERTICAL COMPONENT
        double zAtRest;
        if (posConnector == UP) {
            zAtRest = gsl_vector_get(this->getVecLowerToUpperAtRest(),2);
        } else {
            zAtRest = -gsl_vector_get(this->getVecLowerToUpperAtRest(),2);
        }

        double z = gsl_vector_get(lengthVector,2);

        // Compute the vertical displacment
        double dl = fabs(z) - fabs(zAtRest);

        // Compute the magnitude of the force and set its direction in the direction of z
        double Fmag;
        errCode = this->stiffnessFunctionZ->func(dl, Fmag);
        if (errCode != GSL_SUCCESS){
            return errCode;
        }

        // Set vertical component of the force vector
        if (zAtRest > 0)
            gsl_vector_set(F,2,Fmag);
        else
            gsl_vector_set(F,2,-Fmag);

        // HORIZONTAL COMPONENT
        // Get the horizontal vector
        gsl_vector_view hF_view = gsl_vector_subvector(F,0,2);
        gsl_vector* hF = &hF_view.vector;
        gsl_vector* hDisp = gsl_vector_alloc(2);
        double xDisp = gsl_vector_get(lengthVector,0);
        double yDisp = gsl_vector_get(lengthVector,1);
        gsl_vector_set(hDisp,0,xDisp);
        gsl_vector_set(hDisp,1,yDisp);
        // Compute euclidean norm
        double hl = gsl_blas_dnrm2(hDisp);
        // Compute the force magnitude
        double hFmag;
        errCode = this->stiffnessFunctionXY->func(hl, hFmag);
        if (errCode != GSL_SUCCESS){
            return errCode;
        }
        // Compute unit vector
        if (hl>1e-13) {
            gsl_blas_dscal((1.0/hl), hDisp);
            // Project the force
            gsl_vector_memcpy(hF, hDisp);
            gsl_blas_dscal(hFmag, hF);
        }

        // Free memory
        gsl_vector_free(hDisp);

        return errCode;
    }

    Function* VerticalShearSpring::getStiffnessFunctionZ(){
        return this->stiffnessFunctionZ;
    }
}

