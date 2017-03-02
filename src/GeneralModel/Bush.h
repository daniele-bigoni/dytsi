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
 * File:   Bush.h
 * Author: s083781
 *
 * Created on April 7, 2011, 5:02 PM
 */

#ifndef BUSH_H
#define	BUSH_H

#include "Link.h"
#include "Function.h"

namespace DYTSI_Modelling {
    class IndependentDamper;
    class IndependentSpring;

    class Bush : public Link{
    public:
        Bush(std::string name, gsl_vector* connectionPointUp, gsl_vector* connectionPointDown,
                Function* stiffnessFunctionX, Function* stiffnessFunctionY, Function* stiffnessFunctionZ,
                Function* dampingFunctionX, Function* dampingFunctionY, Function* dampingFunctionZ);
        virtual ~Bush();

        gsl_vector* computeForceAndMoments(
                RelativeComponentPosition posConnector,
                gsl_vector* dispMain, gsl_vector* dispAttached,
                gsl_vector* ddispMain, gsl_vector* ddispAttached,
                gsl_matrix* transformationMatrixMain, gsl_matrix* transformationMatrixAttached,
                gsl_matrix* dTransformationMatrixMain, gsl_matrix* dTransformationMatrixAttached,
                int& errCode);

        IndependentSpring* getIndependentSpring();
        IndependentDamper* getIndependentDamper();
    private:
        IndependentSpring* independentSpring;
        IndependentDamper* independentDamper;

    };
}

#endif	/* BUSH_H */

