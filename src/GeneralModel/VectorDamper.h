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
 * File:   LinearDamper.h
 * Author: s083781
 *
 * Created on February 11, 2011, 4:17 PM
 */

#ifndef LINEARDAMPER_H
#define	LINEARDAMPER_H

#include "LinkDamper.h"

#include <string>

namespace DYTSI_Modelling {

    class VectorDamper : public LinkDamper {
    public:
        VectorDamper( std::string name, gsl_vector* connectionPointUp,
                gsl_vector* connectionPointDown, Function* dampingFunction );
        ~VectorDamper();

        int force (RelativeComponentPosition posConnector, gsl_vector* speed, gsl_vector* lengthVector, gsl_vector* F);
    private:
        Function* dampingFunction;

        //Aux
        gsl_vector* u;
    };
}

#endif	/* LINEARDAMPER_H */

