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
 * File:   IndependentDamper.cpp
 * Author: s083781
 * 
 * Created on April 7, 2011, 4:46 PM
 */

#include "IndependentDamper.h"

#include "../Application/miscellaneous.h"

namespace DYTSI_Modelling {
    IndependentDamper::IndependentDamper(std::string name, gsl_vector* connectionPointUp, gsl_vector* connectionPointDown,
                Function* dampingFunctionX, Function* dampingFunctionY, Function* dampingFunctionZ)
    : LinkDamper( name, connectionPointUp, connectionPointDown) {
        this->dampingFunctionX = dampingFunctionX;
        this->dampingFunctionY = dampingFunctionY;
        this->dampingFunctionZ = dampingFunctionZ;
    }

    IndependentDamper::~IndependentDamper() {
    }

    int IndependentDamper::force (RelativeComponentPosition posConnector, gsl_vector* speed, gsl_vector* lengthVector, gsl_vector* F){
        int errCode = GSL_SUCCESS;

        // All the components are taken in order to get the directions
        double xAtRest;
        if (posConnector == UP) {
            xAtRest = gsl_vector_get(this->getVecLowerToUpperAtRest(),0);
        } else {
            xAtRest = -gsl_vector_get(this->getVecLowerToUpperAtRest(),0);
        }
        double yAtRest;
        if (posConnector == UP) {
            yAtRest = gsl_vector_get(this->getVecLowerToUpperAtRest(),1);
        } else {
            yAtRest = -gsl_vector_get(this->getVecLowerToUpperAtRest(),1);
        }
        double zAtRest;
        if (posConnector == UP) {
            zAtRest = gsl_vector_get(this->getVecLowerToUpperAtRest(),2);
        } else {
            zAtRest = -gsl_vector_get(this->getVecLowerToUpperAtRest(),2);
        }
        double signXAtRest = sign(xAtRest);
        double signYAtRest = sign(yAtRest);
        double signZAtRest = sign(zAtRest);

        double velocityMagnitudeX = gsl_vector_get(speed,0) * signXAtRest;
        double velocityMagnitudeY = gsl_vector_get(speed,1) * signYAtRest;
        double velocityMagnitudeZ = gsl_vector_get(speed,2) * signZAtRest;

        // Compute the resulting magnitude of the force
        double FmagX;
        errCode = this->dampingFunctionX->func(velocityMagnitudeX, FmagX);
        if (errCode != GSL_SUCCESS){
            return errCode;
        }
        double FmagY;
        errCode = this->dampingFunctionY->func(velocityMagnitudeY,FmagY);
        if (errCode != GSL_SUCCESS){
            return errCode;
        }
        double FmagZ;
        errCode = this->dampingFunctionZ->func(velocityMagnitudeZ, FmagZ);
        if (errCode != GSL_SUCCESS){
            return errCode;
        }

        // Set the force vector
        gsl_vector_set(F,0,FmagX * signXAtRest);
        gsl_vector_set(F,1,FmagY * signYAtRest);
        gsl_vector_set(F,2,FmagZ * signZAtRest);

        return errCode;
    }
}
