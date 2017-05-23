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
 * File:   LongitudinalDamper.cpp
 * Author: s083781
 * 
 * Created on April 7, 2011, 3:42 PM
 */

#include "LongitudinalDamper.h"

#include "../Application/miscellaneous.h"

namespace DYTSI_Modelling {
    LongitudinalDamper::LongitudinalDamper(std::string name, gsl_vector* connectionPointUp,
                gsl_vector* connectionPointDown, Function* dampingFunction)
    : LinkDamper( name, connectionPointUp, connectionPointDown) {
        this->dampingFunction = dampingFunction;
    }

    LongitudinalDamper::~LongitudinalDamper() {
    }

    int LongitudinalDamper::force (RelativeComponentPosition posConnector, gsl_vector* speed, gsl_vector* lengthVector, gsl_vector* F){
        int errCode = GSL_SUCCESS;

        // Take the vertical component in order to get the direction of the link
        double xAtRest;
        if (posConnector == UP) {
            xAtRest = gsl_vector_get(this->getVecLowerToUpperAtRest(),0);
        } else {
            xAtRest = -gsl_vector_get(this->getVecLowerToUpperAtRest(),0);
        }
        double signXAtRest = sign(xAtRest);

        // The desired component of the speed is taken
        double velocityMagnitude = gsl_vector_get(speed,0) * signXAtRest;

        // Compute the resulting magnitude of the force
        double Fmag;
        errCode = this->dampingFunction->func(velocityMagnitude, Fmag);
        if (errCode != GSL_SUCCESS){
            return errCode;
        }

        // Set the force vector
        gsl_vector_set_zero(F);
        gsl_vector_set(F,0, Fmag * signXAtRest);

        return errCode;
    }
}

