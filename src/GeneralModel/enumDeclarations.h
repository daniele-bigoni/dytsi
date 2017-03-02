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
 * File:   enumDeclarations.h
 * Author: s083781
 *
 * Created on February 11, 2011, 4:03 PM
 */

#ifndef ENUMDECLARATIONS_H
#define	ENUMDECLARATIONS_H

namespace DYTSI_Modelling {
    enum RelativeComponentPosition { UP, LL, LT };
    enum ComponentPosition { LEADING, TRAILING };

    enum InterpolationTypes { INTP_LINEAR, INTP_CUBICSPLINE, INTP_AKIMA };
    enum FunctionTypes { FT_LINEAR, FT_PIECEWISELINEAR, FT_PWLEVENT };

//    enum WheelSetComponent::WheelSet_DOF { Q1, Q1DOT, Q2, Q2DOT, Q3, Q3DOT, Q4, Q4DOT, BETA};
//    enum BogieFrameComponent::BogieFrame_DOF { Q1, Q1DOT, Q2, Q2DOT, Q3, Q3DOT, Q4, Q4DOT, Q5, Q5DOT};
//    enum CarBodyComponent::CarBody_DOF { Q1, Q1DOT, Q2, Q2DOT, Q3, Q3DOT, Q4, Q4DOT, Q5, Q5DOT};
}

#endif	/* ENUMDECLARATIONS_H */

