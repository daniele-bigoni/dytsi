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
 * Incluce file for step class                    *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#ifndef __step_h
#define __step_h

#include "divctrl.h"
#include "stepbase.h"

/*************************************************/
class OrdStepControl : public StepControlBase
{
private:
  // Pointer to the DivergensStepControl class
  DivergensStepControl *DivergensControl;
public:
  // Constructor
  OrdStepControl(double deps, int iorder, int in);

  // These virtuals are overriding the public functions
  // defined in StepControlBase (stepbase.h)
  virtual BOOL NewStepLength(DVector &trunc_error,
			     BOOL newton_divergens,
			     double roc, double &h);
  virtual void Reset();
  virtual void SetSafeFac(double deps);
  virtual void SetPiConstant(PiCoefType &ok, PiCoefType &reject);
};

/*************************************************/
class PrimStepControl : public StepControlBase
{
public:
  //Constructor
  PrimStepControl(double deps, int iorder, int in);

  // These virtuals are overriding the public functions
  // defined in StepControlBase (stepbase.h)
  virtual BOOL NewStepLength(DVector &trunc_error,
			     BOOL newton_divergens,
			     double roc, double &h);
  virtual void Reset();
  virtual void SetSafeFac(double deps);
  virtual void SetPiConstant(PiCoefType &ok, PiCoefType &reject);
};


/*************************************************/
class PiStepControl : public StepControlBase
{
protected:
  double Integral, OldError;
  double K_1, K_2;
  BOOL FirstStep;
  PiCoefType PiConstOk, PiConstReject;
  DivergensStepControl *DivergensControl;
public:
  // Constructor
  PiStepControl(double deps, int iorder, int in);

  // These virtuals are overriding the public functions
  // defined in StepControlBase (stepbase.h)
  virtual BOOL NewStepLength(DVector &trunc_error,
			     BOOL newton_divergens,
			     double roc, double &h);
  virtual void Reset();
  virtual void SetSafeFac(double deps);
  virtual void SetPiConstant(PiCoefType &ok, PiCoefType &reject);
};

/*************************************************/
class WattsStepControl : public PiStepControl
// PiStepControl defined just above
{
protected:
  double H_1, H_2;
public:
  // Constructor
  WattsStepControl(double deps, int iorder, int in);

  // These virtuals are overriding the public functions
  // defined in PiStepControl (just above)
  virtual BOOL NewStepLength(DVector &trunc_error,
			     BOOL newton_divergens,
			     double roc, double &h);
  virtual void Reset();
};

#endif




