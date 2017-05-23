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
 * File:   VerticalSpring.cpp
 * Author: s083781
 * 
 * Created on April 7, 2011, 12:11 PM
 */

#include "VerticalSpring.h"
#include "Link.h"
#include "Function.h"

#include <cmath>

namespace DYTSI_Modelling {
    VerticalSpring::VerticalSpring(std::string name, gsl_vector* connectionPointUp,
            gsl_vector* connectionPointDown, Function* stiffnessFunction)
    : LinkSpring(name, connectionPointUp, connectionPointDown){
        this->stiffnessFunction = stiffnessFunction;
    }

    VerticalSpring::~VerticalSpring() {
    }

    int VerticalSpring::force(RelativeComponentPosition posConnector,
            gsl_vector* lengthVector, gsl_vector* F){
        int errCode = GSL_SUCCESS;

        // Only the vertical component is needed an computed
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
        errCode = this->stiffnessFunction->func(dl, Fmag);
        if (errCode != GSL_SUCCESS)
            return errCode;

        // Compute the Force vector
        gsl_vector_set_zero(F);
        if (zAtRest > 0)
            gsl_vector_set(F,2,Fmag);
        else
            gsl_vector_set(F,2,-Fmag);

        return errCode;
    }

    Function* VerticalSpring::getStiffnessFunctionZ(){
        return this->stiffnessFunction;
    }
}
