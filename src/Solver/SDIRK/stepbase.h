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
 * Include file for step control class            *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#ifndef __stepbase_h
#define __stepbase_h

#include "dvector.h"
#include "types.h"

class StepControlBase
{
protected:
  int n;
  double Eps, HRoc, MaxError;
  int Order;
  BOOL DivergensInThisStep;
public:
  // Constructor
  StepControlBase(double deps, int iorder, int in);
  // Set tolerance
  void SetEps(double deps);
  // Calculate the estimated maximal error
  double GetMaxError();
  // Reset the controls, enabling re-start of integration
  virtual void Reset();
  // Set Pi
  virtual void SetPiConstant(PiCoefType &ok, PiCoefType &reject);
  // Set the safety factor
  virtual void SetSafeFac(double deps);
  // Calculate the new step length
  virtual BOOL NewStepLength(DVector &trunc_error, BOOL newton_divergens,
			     double it_speed, double &h);
};
#endif

