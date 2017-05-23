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
 * File:   Bush.cpp
 * Author: s083781
 * 
 * Created on April 7, 2011, 5:02 PM
 */

#include "Bush.h"
#include "IndependentDamper.h"
#include "IndependentSpring.h"

#include <gsl/gsl_blas.h>

namespace DYTSI_Modelling {
    Bush::Bush(std::string name, gsl_vector* connectionPointUp, gsl_vector* connectionPointDown,
                Function* stiffnessFunctionX, Function* stiffnessFunctionY, Function* stiffnessFunctionZ,
                Function* dampingFunctionX, Function* dampingFunctionY, Function* dampingFunctionZ)
    : Link(name, connectionPointUp, connectionPointDown){
        std::string springName(name);
        springName.append(" - Spring");
        this->independentSpring = new IndependentSpring(springName, connectionPointUp, connectionPointDown,
                stiffnessFunctionX, stiffnessFunctionY, stiffnessFunctionZ );

        std::string damperName(name);
        damperName.append(" - Damper");
        this->independentDamper = new IndependentDamper(damperName, connectionPointUp, connectionPointDown,
                dampingFunctionX, dampingFunctionY, dampingFunctionZ );
    }

    Bush::~Bush() {
    }

    gsl_vector* Bush::computeForceAndMoments(
                RelativeComponentPosition posConnector,
                gsl_vector* dispMain, gsl_vector* dispAttached,
                gsl_vector* ddispMain, gsl_vector* ddispAttached,
                gsl_matrix* transformationMatrixMain, gsl_matrix* transformationMatrixAttached,
                gsl_matrix* dTransformationMatrixMain, gsl_matrix* dTransformationMatrixAttached,
                int& errCode) {
        gsl_vector* out = gsl_vector_calloc(6);
        
        // Compute the forces due to spring and damping component
        gsl_vector* springForces = this->independentSpring->computeForceAndMoments(posConnector, dispMain, dispAttached, ddispMain,
                ddispAttached, transformationMatrixMain, transformationMatrixAttached, dTransformationMatrixMain,
                dTransformationMatrixAttached, errCode);
        if (errCode != GSL_SUCCESS){ return out; }
        gsl_vector* damperForces = this->independentDamper->computeForceAndMoments(posConnector, dispMain, dispAttached, ddispMain,
                ddispAttached, transformationMatrixMain, transformationMatrixAttached, dTransformationMatrixMain,
                dTransformationMatrixAttached, errCode);
        if (errCode != GSL_SUCCESS){ return out; }

        // Take the sum of the forces
        gsl_vector_memcpy(out, springForces);
        gsl_blas_daxpy(1.0, damperForces, out);

        // Free the memory
        gsl_vector_free(springForces);
        gsl_vector_free(damperForces);

        return out;
    }

    IndependentSpring* Bush::getIndependentSpring() {
        return this->independentSpring;
    }

    IndependentDamper* Bush::getIndependentDamper(){
        return this->independentDamper;
    }
}

