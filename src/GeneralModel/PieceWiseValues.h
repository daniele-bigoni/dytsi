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
 * File:   PieceWiseValues.h
 * Author: s083781
 *
 * Created on May 2, 2011, 2:55 PM
 */

#ifndef PIECEWISEVALUES_H
#define	PIECEWISEVALUES_H

#include "enumDeclarations.h"

#include <string>

namespace DYTSI_Modelling {
    class PieceWiseValues {
    public:
        PieceWiseValues(double** vals, int size, std::string interpType = "linear");
        virtual ~PieceWiseValues();

        double** getVals() { return vals; }
        int getSize() { return size; }
        DYTSI_Modelling::InterpolationTypes getInterpType();
    private:
        double** vals;
        int size;
        InterpolationTypes interpType;
    };
}

#endif	/* PIECEWISEVALUES_H */

