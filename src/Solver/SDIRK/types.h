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
 */

/**************************************************
 * Incluce file for usertypes and definitions     *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#ifndef __types_h
#define __types_h

#include <iostream>
#include <math.h>
#include <stdlib.h>
// For the Newton iteration the tolerence DEF_NEWTON_FAC*Eps is used
// (a 100th of the integration overall tolerance)
#define DEF_NEWTON_FAC     0.01
// Safety factor
#define DEF_SAFE_FAC       0.98
// The Newton iterations are divergent if ROC >= DEF_DIVERGENS_ROC
#define DEF_DIVERGENS_ROC  0.75
// The Jacobian is recalculated and facturized if ROC >= DEF_NEW_JACOBI_ROC
#define DEF_NEW_JACOBI_ROC 0.65
// Maximal number of iterations performed in the Newton iteration
#define DEF_MAX_ITERATIONS 20
// DBL_EPSILON is not defined on all systems
#ifndef DBL_EPSILON
#define DBL_EPSILON 2.2204e-16
#endif

enum BOOL            {FALSE, TRUE};
enum VecError        {V_ALLOC, V_RANGE, V_NOTDIM, V_NOT_IDENT};
enum MatError        {M_ALLOC, M_RANGE, M_NOTDIM, M_NOT_IDENT};
enum RKError         {SE_STEP_TO_SMALL, SE_ALLOC};
enum ERROR_TYPE      {OK, NO_CONVG, SINGUL};
enum NormType        {NT_ONE, NT_TWO, NT_INF};
enum StepControlType {SC_PRIM, SC_ORDINARE, SC_PI, SC_WATTS};
enum SDIRKMethodType {NT1,JKT,RKF34};
enum RKType          {EXPLICIT,IMPLICIT};

struct SdirkInfoType{
  unsigned long int NumOfGoodStep, NumOfBadStep, NumOfNewtonDivergens;
  double MaxError;
};

struct PiCoefType{
  double Proportional, Integral, ThetaMax, Rho;
};

#endif
