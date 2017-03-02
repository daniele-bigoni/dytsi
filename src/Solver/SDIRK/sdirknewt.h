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
 * Include file for Sdirk NewtonRaphson class     *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#ifndef __sdirknewt_h
#define __sdirknewt_h

#include "newtbase.h"
#include "sdirk.h"

// Class predefinition
// (class SdirkNewtonRaphson needs it - it is a friend of SdirkNewtonRaphson)
class Sdirk;

class SdirkNewtonRaphson : public NewtonRaphsonBase {
private:
  // Boolean. Is this the first step?
  BOOL FirstStep;
  // Pointer to the Sdirk class
  Sdirk *Integrator;
  // Step length variables
  double ThisH, LastH;
  // Perform the Newton-Raphson iteration
  int NewtonFunction(DVector &y_in, int stage, DVector &g_out);
  // Calculate the Newton-Raphson matrix
  int NewtonMatrix(int stage, DVector &y);
  // Calculate an initial guess for the iteration
  void InitialGuess(int stage, DVector &guess);
public:
  // Constructor
  SdirkNewtonRaphson(Sdirk *p,
		     double accur,
		     int dimen,
		     int(* fun)(double t, DVector &y, DVector &f),
		     int(* jac)(double t, DVector &y, DMatrix &j));
  // Calculate an iteration
  int Iterate(int stage, DVector &y_out, double &it_speed, int &div);
};
#endif
