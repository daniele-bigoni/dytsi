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
 * Source file for step class                     *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#include "step.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b) )

double Alpha;
void SetSafeFac(double fac){Alpha = fac;}

//************************************************
// OrdStepControl methods
//************************************************
// Constructor
OrdStepControl::OrdStepControl(double deps, int iorder, int in)
  : StepControlBase(deps, iorder, in)
{
  SetSafeFac(deps);
  DivergensControl = new DivergensStepControl();
  if(DivergensControl == NULL){
    std::cerr << "SDIRK: Memory error in OrdStepControl.\n";
    exit(0);
  }  
}

// Set safety factor
void OrdStepControl::SetSafeFac(double deps){
  if(deps >= 1e-4)
    Alpha = 0.8;
  else
    if(deps >= 1e-6)
      Alpha = 0.9;
    else
      if(deps >= 1e-8)
	Alpha = 0.97;
      else
	Alpha = 1.0;
}

void OrdStepControl::Reset(){
  MaxError = 0.0;
}

// Step length control evaluation
// The step length is controled by an error estimate and ROC
// The function returns FALSE if the step should be recalculated,
// TRUE if the step length is acceptable
BOOL OrdStepControl::NewStepLength(DVector &trunc_error,
				   BOOL newton_divergens,
				   double roc, double &h)
{
  double beta, error;
  double h_r, h_roc, h_temp;
  BOOL step_ok;
  
  h_roc = h;
  DivergensControl->NewStepLength(newton_divergens, roc, h_roc);
  if(newton_divergens){
    h = h_roc;
    return FALSE;
  }
  
  error = trunc_error.Norm(NT_TWO);

  if(error > MaxError)
    MaxError = error;

  if(error > 1.2*Eps)
    step_ok = FALSE;
  else
    step_ok = TRUE;
  
  if(error > DBL_EPSILON)
    beta = pow( Eps/error , 1.0/Order );
  else
    beta = 2.0;

  h_temp = beta*h;
  if(h_temp > 2.0*h)
    h_r = 2.0*h;
  else
    h_r = Alpha*h_temp;
  
  h =  min(h_r, h_roc);
  return step_ok;
}

// Dummy function to avoid all the warnings
void OrdStepControl::SetPiConstant(PiCoefType &ok, PiCoefType &reject)
{
  PiCoefType dummy;
  dummy.Rho = ok.Rho;
  dummy.Rho = reject.Rho;
}








//************************************************
// PrimStepControl methods
//************************************************
// Constructor
PrimStepControl::PrimStepControl(double deps, int iorder, int in)
  : StepControlBase(deps, iorder, in)
{
  SetSafeFac(deps);
}

// Set safety factor
void PrimStepControl::SetSafeFac(double deps)
{
  if(deps >= 1e-4)
    Alpha = 0.8;
  else
    if(deps >= 1e-6)
      Alpha = 0.9;
    else
      if(deps >= 1e-8)
	Alpha = 0.97;
      else
	Alpha = 1.0;
}

void PrimStepControl::Reset(){
  MaxError = 0.0;
}

// Step length control evaluation
// The step length is controled by an error estimate and ROC
// The function returns FALSE if the step should be recalculated,
// TRUE if the step length is acceptable
BOOL PrimStepControl::NewStepLength(DVector &trunc_error,
				    BOOL newton_divergens,
				    double roc, double &h)
{
  double beta, error;
  double h_r, h_temp;
  BOOL step_ok;
  
  if(newton_divergens){
    h = 0.7*h;
    return FALSE;
  }
  
  error = trunc_error.Norm(NT_TWO);

  if(error > MaxError)
    MaxError = error;

  if(error > 1.2*Eps)
    step_ok = FALSE;
  else
    step_ok = TRUE;
  
  if(error > DBL_EPSILON)
    beta = pow( Eps/error , 1.0/Order);
  else
    beta = 2.0;

  h_temp = beta*h;
  if(h_temp > 2.0*h)
    h_r = 2.0*h;
  else
    h_r = Alpha*h_temp;
  h = h_r;
  return step_ok;
}

// Dummy function to avoid all the warnings
void PrimStepControl::SetPiConstant(PiCoefType &ok, PiCoefType &reject)
{
  PiCoefType dummy;
  dummy.Rho = ok.Rho;
  dummy.Rho = reject.Rho;
}





//************************************************
// PiStepControl methods
//************************************************
// Constructor
PiStepControl::PiStepControl(double deps, int iorder, int in)
  : StepControlBase(deps, iorder, in){ 
  DivergensControl = new DivergensStepControl();
  if(DivergensControl == NULL){
    std::cerr << "SDIRK: Memory error in PiStepControl\n";
    exit(0);
  }
  Reset();
  SetSafeFac(deps);
  PiConstReject.Proportional = 0.0;
  PiConstReject.Integral     = 0.2;
  PiConstReject.ThetaMax     = 2.0;
  PiConstReject.Rho          = 1.2;
  
  PiConstOk.Proportional = 0.4/iorder;
  PiConstOk.Integral     = 0.3/iorder;
  PiConstOk.ThetaMax     = 2.0;
  PiConstOk.Rho          = 1.2;
}

// Set Pi
void PiStepControl::SetPiConstant(PiCoefType &ok, PiCoefType &reject)
{
  PiConstReject.Proportional = reject.Proportional;
  PiConstReject.Integral     = reject.Integral;
  PiConstReject.ThetaMax     = reject.ThetaMax;
  PiConstReject.Rho          = reject.Rho;
  
  PiConstOk.Proportional = ok.Proportional;
  PiConstOk.Integral     = ok.Integral;
  PiConstOk.ThetaMax     = ok.ThetaMax;
  PiConstOk.Rho          = ok.Rho;  
}

// Set safety factor
void PiStepControl::SetSafeFac(double deps)
{
  if(deps >= 1e-4)
    Alpha = 0.8;
  else
    if(deps >= 1e-6)
      Alpha = 0.9;
    else
      if(deps >= 1e-8)
	Alpha = 0.97;
      else
	Alpha = 1.0;
  Alpha = 1.0;
}

void PiStepControl::Reset(){
  Integral = 0.0;
  MaxError = 0.0;
  FirstStep = TRUE;
}

// Pi step length control evaluation
// The step length is controled by an error estimate and ROC
// The function returns FALSE if the step should be recalculated,
// TRUE if the step length is acceptable
BOOL PiStepControl::NewStepLength(DVector &trunc_error,
				  BOOL newton_divergens,
				  double roc, double &h){
  double e, error, h_temp, intg_temp, prop;
  double h_r, h_roc;
  BOOL step_ok;
  PiCoefType *pi;
  
  h_roc = h;
   DivergensControl->NewStepLength(newton_divergens, roc, h_roc);
  if(newton_divergens){
    h = h_roc;
    return FALSE;
  }
  
  error = trunc_error.Norm(NT_TWO);
  if(error < DBL_EPSILON)
    error = DBL_EPSILON;
  if(error > MaxError)
    MaxError = error;
  if(error > 1.2*Eps){
    pi = &PiConstReject;
    step_ok = FALSE;
  }
  else{
    step_ok = TRUE;
    pi = &PiConstOk;
  }

  if(FirstStep){
    h_temp = pow(Eps/error, pi->Integral) * h;
    FirstStep = FALSE;
  }
  else
    h_temp = pow(Eps/error, pi->Integral) *
      pow(OldError/error, pi->Proportional)*h;
  if(h_temp > pi->ThetaMax * h)
    h_r = pi->ThetaMax * h;
  else
    h_r = Alpha*h_temp;
  h = min(h_r, h_roc);
  OldError = error;
  return step_ok;
}

//************************************************
// WattsStepControl methods
//************************************************
// Constructor
WattsStepControl::WattsStepControl(double deps, int iorder, int in)
  : PiStepControl(deps, iorder, in)
{
  Reset();
  PiConstReject.Proportional = 0.0 ;
  PiConstReject.Integral     = 0.2;
  PiConstReject.ThetaMax     = 2.0;
  PiConstReject.Rho          = 1.2;
  
  // Set constants
  K_1 = 1.0; K_2 = 0.6;
  PiConstOk.Proportional = K_2 / double(iorder);
  PiConstOk.Integral     = K_1 / double(iorder);
  PiConstOk.ThetaMax     = 2.0;
  PiConstOk.Rho          = 1.2;
}

// Reset the control
void WattsStepControl::Reset()
{
  H_1 = H_2 = 1.0;
  Integral = 0.0;
  MaxError = 0.0;
  FirstStep = TRUE;
}

// Watts step length control evaluation
// The step length is controled by an error estimate and ROC
// The function returns FALSE if the step should be recalculated,
// TRUE if the step length is acceptable
BOOL WattsStepControl::NewStepLength(DVector &trunc_error,
				     BOOL newton_divergens,
				     double roc, double &h){
  double e, error, h_temp, intg_temp, prop;
  double h_r, h_roc;
  BOOL step_ok;
  PiCoefType *pi;
  
  h_roc = h;
  DivergensControl->NewStepLength(newton_divergens, roc, h_roc);
  if(newton_divergens){
    h = h_roc;
    return FALSE;
  }
  
  error = trunc_error.Norm(NT_TWO);
  if(error < DBL_EPSILON)
    error = DBL_EPSILON;
  if(error > MaxError)
    MaxError = error;
  
  if(error > 1.2*Eps){
    pi = &PiConstReject;
    step_ok = FALSE;
  }
  else{
    step_ok = TRUE;
    pi = &PiConstOk;
  }
  
  if(FirstStep){
    h_temp = pow(Eps/error, pi->Integral) * h;
    FirstStep = FALSE;
  }
  else
    h_temp =H_1/H_2*pow(Eps/error, pi->Integral) *
      pow(OldError/error, pi->Proportional)*h;
  
  if(h_temp > pi->ThetaMax * h)
    h_r = pi->ThetaMax * h;
  else
    h_r = Alpha*h_temp;
  
  H_2 = H_1;
  H_1 = h;
  OldError = error;
  h = min(h_r, h_roc);
  return step_ok;
}

