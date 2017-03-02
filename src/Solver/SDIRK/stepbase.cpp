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
 * Source file for step control class             *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#include "stepbase.h"

// Base constructor
StepControlBase::StepControlBase(double deps, int iorder, int in){
  Eps = deps;
  Order = iorder;
  n = in;
  DivergensInThisStep = FALSE;
}

// Set the safety factor
void StepControlBase::SetEps(double deps){
  if (deps*DEF_NEWTON_FAC<=DBL_EPSILON){
    std::cout << "SDIRK: Error in SetEps.\nEps must be " << 1/DEF_NEWTON_FAC;
    std::cout << " times larger than the unit roundoff " << DBL_EPSILON << "\n";
    exit(0);
  }
  Eps = deps;
  Reset();
  SetSafeFac(Eps);
};

// Calculate the estimated maximal truncation error
double StepControlBase::GetMaxError(){
  return MaxError;
}

// Reset the control
void StepControlBase::Reset(){};

// Set Pi
void StepControlBase::SetPiConstant(PiCoefType &ok, PiCoefType &reject){};

// Set the safety factor
void StepControlBase::SetSafeFac(double deps){};

// Evaluate the new step length
BOOL StepControlBase::NewStepLength(DVector &trunc_error,
				    BOOL newton_divergens,
				    double it_speed, double &h){};

