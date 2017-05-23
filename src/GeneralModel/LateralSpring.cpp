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
 * File:   LateralSpring.cpp
 * Author: s083781
 * 
 * Created on April 7, 2011, 12:38 PM
 */

#include "LateralSpring.h"

#include "LinearFunction.h"

#include <cmath>

namespace DYTSI_Modelling {
    LateralSpring::LateralSpring(std::string name, gsl_vector* connectionPointUp,
            gsl_vector* connectionPointDown, Function* stiffnessFunction)
    : LinkSpring(name, connectionPointUp, connectionPointDown) {
        this->stiffnessFunction = stiffnessFunction;
        this->stiffnessFunctionZ = new LinearFunction(0.0);
    }

    LateralSpring::~LateralSpring() {
    }

    int LateralSpring::force(RelativeComponentPosition posConnector,
            gsl_vector* lengthVector, gsl_vector* F){
        int errCode = GSL_SUCCESS;

        // Only the lateral component is needed an computed
        double yAtRest;
        if (posConnector == UP) {
            yAtRest = gsl_vector_get(this->getVecLowerToUpperAtRest(),1);
        } else {
            yAtRest = -gsl_vector_get(this->getVecLowerToUpperAtRest(),1);
        }

        double y = gsl_vector_get(lengthVector,1);

        // Compute the vertical displacment
        double dl = fabs(y) - fabs(yAtRest);

        // Compute the magnitude of the force
        double Fmag;
        errCode = this->stiffnessFunction->func(dl, Fmag);
        if (errCode != GSL_SUCCESS){
            return errCode;
        }

        // Compute the Force vector
        gsl_vector_set_zero(F);
        if (yAtRest > 0)
            gsl_vector_set(F,1,Fmag);
        else
            gsl_vector_set(F,1,-Fmag);

        return errCode;
    }

    Function* LateralSpring::getStiffnessFunctionZ(){
        return this->stiffnessFunctionZ;
    }
}
