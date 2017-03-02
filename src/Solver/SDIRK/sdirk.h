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
 * Include file for Sdirk class                   *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#ifndef __sdirk_h
#define __sdirk_h

#include "rkbase.h"
#include "step.h"
#include "sdirknewt.h"

// Class predefinition
// (class Sdirk needs it - it is a friend of Sdirk)
class SdirkNewtonRaphson;

class Sdirk : public RKbase {
private:
  // Pointer to step controller
  StepControlBase *StepControl;
  // Pointer to Newton Raphson iterator
  SdirkNewtonRaphson *Newton;
  // Iteration matrix
  DMatrix OldK;
  // Boolean. Is this iteration the first iteration?
  BOOL FirstStep;
  // Declare an SdirkInfoType instance named 'Info'
  SdirkInfoType Info;
  // Declare RKType
  RKType rktype;
  // Calculate solution
  void Solution(DVector &y_out);
  // Calculate truncation error
  void TruncError();
  // Set Pi used in step length control
  void SetPiConstant(PiCoefType &ok, PiCoefType &reject);
  // Select RK method
  // For the time being, only one method is supplied. If more gets 
  // implementes, this call should be public
  void SelectMethod(SDIRKMethodType m, RKcoef &p);
public:
  // SdirkNewtonRaphson is a friend of Sdirk.
  // It hereby has access to its private part
  friend class SdirkNewtonRaphson;
  // Constructor
  Sdirk(double accur,
	int num_ode,
	int(* fun)(double t, DVector &y, DVector &f),
	int(* jac)(double t, DVector &y, DMatrix &j),
	StepControlType ctrl,
        SDIRKMethodType methodType);
  // Destructor
  ~Sdirk();
  // Integrate single step
  int Integrate(double &t, double &h, DVector &y);
  //  Integrate from t_lo upto t_hi
  int Integrate(DVector &t, double &h, DVector &y, long &Nstep);
  // Apply a stage of the method
  int ApplyStage(int stage, DVector &y_out);
  // Reset integrator and set FirstStep = TRUE
  void Reset();
  // Set new value for Eps
  void SetEps(double eps);
  // Get information about the recent integration
  void GetInfo(SdirkInfoType &p);
  // Show information in a nice way
  void ShowInfo();
};
#endif
