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
 * Source file for Runge-Kutta integration class  *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#include "rkbase.h"

// Prototype for default errorhandler
static void DefaultErrorHandler(RKError err);

// Default errorhandler
static void DefaultErrorHandler(RKError err)
{
  switch(err){
  case SE_STEP_TO_SMALL :
    std::cerr << "SDIRK: Steplength in object Sdirk to small. h<DBL_EPSILON\n";
    std::cerr << "SDIRK: Use bigger Eps\n";
    break;
  case SE_ALLOC :
    std::cerr << "SDIRK: Memory allocation error. Insufficient memory.\n";
    break;
  }
  std::cout <<"\n";
  exit(0);
}

// Constructor
RKbase::RKbase(double accur, int num_ode,
               int(* fun)(double t, DVector &y, DVector &f) )
{
  N = num_ode;
  UserF = fun;
  AssignHandler(&DefaultErrorHandler);
  Error.Dimension(1,N);
  StepY.Dimension(1,N);
}

// Dummy function to avoid all the warnings
void RKbase::Solution(DVector &y_out)
{
  double dummy;
  dummy = y_out[1];
}

// Dummy function to avoid all the warnings
void RKbase::SelectMethod(int m, RKcoef &p)
{
  RKcoef dummy;
  dummy.order = p.order;
  int dummy2 = m;
  dummy2++;
}
