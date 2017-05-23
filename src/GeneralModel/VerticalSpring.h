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
 * File:   VerticalSpring.h
 * Author: s083781
 *
 * Created on April 7, 2011, 12:11 PM
 */

#ifndef VERTICALSPRING_H
#define	VERTICALSPRING_H

#include "LinkSpring.h"

namespace DYTSI_Modelling {
    class VerticalSpring : public LinkSpring {
    public:
        VerticalSpring(std::string name, gsl_vector* connectionPointUp,
            gsl_vector* connectionPointDown, Function* stiffnessFunction);
        virtual ~VerticalSpring();

        int force(RelativeComponentPosition posConnector,
                gsl_vector* lengthVector, gsl_vector* F);

        Function* getStiffnessFunctionZ();

    private:
        Function* stiffnessFunction;
    };
}

#endif	/* VERTICALSPRING_H */

