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
 * Source file for Newton Raphson iteration class *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#include "newtbase.h"

// Constructor
NewtonRaphsonBase::NewtonRaphsonBase
(double accur, int dimen,
 int(* fun)(double t, DVector &y, DVector &f),
 int(* jac)(double t, DVector &y, DMatrix &j)
 )
{
  Eps = accur;
  N = dimen;
  UserF = fun;
  UserJacobi = jac;
  Jacobi.Dimension(1,N,1,N);
  LinSolver = new LUfactorize(N);
}

// Dummy function to avoid all the warnings
// Exact definition if found in the source of the derived: sdirknewt.cc
void NewtonRaphsonBase::NewtonMatrix(DMatrix &jacobi)
{
  double dummy=jacobi(1,1); dummy++;
}

// Dummy function to avoid all the warnings
// Exact definition if found in the source of the derived: sdirknewt.cc
void NewtonRaphsonBase::InitialGuess(DMatrix &guess)
{
  double dummy=guess(1,1); dummy++;
}





