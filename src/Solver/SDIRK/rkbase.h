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
 * Include file for Runge-Kutta integration class *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#ifndef __rkbase_h
#define __rkbase_h

#include "dmatrix.h"
#include "dvector.h"
#include "ivector.h"

class RKbase
{
protected:
  // Number of ODE's
  int N;
  // Step length indicators
  double StepX, StepH, NextH;
  // Iteration matrices
  DMatrix K;
  DVector Error, StepY;
  // Pointer to righthand side function
  int(* UserF)(double t, DVector &y_in, DVector &y_out);
  // Pointer to errorhandler
  void( *ErrorHandler)(RKError err);
  // Butcher Tableau
  struct RKcoef
  {
    int order, stage;
    DMatrix a;
    DVector b, c, d;
  };
  // Declare Method as an instance of type RKcoef
  RKcoef Method;
public:
  // Constructor
  RKbase(double accur, int num_ode,
	 int(* fun)(double t, DVector &y, DVector &f) );
  // Integrate the system, one RK iteration
  virtual int Integrate(double &t, double &h, DVector &y) = 0;
  // Assign a user errorhandler
  void AssignHandler(void (*user_handler)(RKError err)){
    ErrorHandler = user_handler;
  }
  // Calculate estimated truncation error
  virtual void TruncError() {};
  // Calculate a solution for a RK iteration
  virtual void Solution(DVector &y_out);
  // Select Butcher Tableau to be used
  virtual void SelectMethod(int m, RKcoef &p);
};
#endif




