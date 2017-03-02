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

/***************************************************
 * Include file for Newton Raphson iteration class *
 * Implemented in MS-DOS 1991 by Michael Jeppesen  *
 * Ported to UNIX spring 1998 by Erik Ostergaard   *
 ***************************************************/

#ifndef __newtbase_h
#define __newtbase_h

#include "lufac.h"

class NewtonRaphsonBase
{
protected:
  // Number of unknowns
  int N;
  // Accuracy used in iteration
  double Eps;
  // Jacobi matrix
  DMatrix Jacobi;
  // Pointer to usersupplied ODE system
  int(* UserF)(double t, DVector & y, DVector &f);
  // Pointer to usersupplied Jacobian
  int(* UserJacobi)(double t, DVector &y, DMatrix &jacobi);
  // Pointer to the linear system solver
  LUfactorize *LinSolver;
public:
  // Constructor
  NewtonRaphsonBase(double accur, int dimen,
		    int(* fun)(double t, DVector &y, DVector &f),
		    int(* jac)(double t, DVector &y, DMatrix &j));
  // Calculate the Newton matrix  
  virtual void NewtonMatrix(DMatrix &jacobi);
  // Calculate an initial guess for the iterations
  virtual void InitialGuess(DMatrix &guess);
  
};
#endif







