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
 * File:   Damper.h
 * Author: s083781
 *
 * Created on February 11, 2011, 4:16 PM
 */

#ifndef DAMPER_H
#define	DAMPER_H

#include <gsl/gsl_vector.h>
#include "Link.h"
#include "Function.h"

#include <string>

namespace DYTSI_Modelling {

    class LinkDamper : public Link {

    public:
        LinkDamper(std::string name, gsl_vector* connectionPointUp, gsl_vector* connectionPointDown);
        ~LinkDamper();

        gsl_vector* computeForceAndMoments(
                RelativeComponentPosition posConnector,
                gsl_vector* dispMain, gsl_vector* dispAttached,
                gsl_vector* ddispMain, gsl_vector* ddispAttached,
                gsl_matrix* transformationMatrixMain, gsl_matrix* transformationMatrixAttached,
                gsl_matrix* dTransformationMatrixMain, gsl_matrix* dTransformationMatrixAttached,
                int& errCode);

        virtual int force (RelativeComponentPosition posConnector, gsl_vector* speed, gsl_vector* lengthVector, gsl_vector* F) = 0;

    private:
        gsl_vector* rMainDamper;
        gsl_vector* rAttachedDamper;
        gsl_vector* length;
        gsl_vector* drMainDamper;
        gsl_vector* drAttachedDamper;
        gsl_vector* v;
        gsl_vector* F;
        gsl_vector* F_rel;
        gsl_vector* M;
        gsl_vector* Fb;
        gsl_vector* vecForceAndMomentum;
    };
}

#endif	/* DAMPER_H */

