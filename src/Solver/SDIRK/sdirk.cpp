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
 * Source file for Sdirk class                    *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#include "sdirk.h"
#include <gsl/gsl_errno.h>

// Constructor
Sdirk::Sdirk(double accur,
	     int num_ode,
             int(* fun)(double t, DVector &y, DVector &f),
             int(* jac)(double t, DVector &y, DMatrix &j),
             StepControlType ctrl,
             SDIRKMethodType methodType)
 : RKbase(accur, num_ode, fun)
{ 
  if (accur*DEF_NEWTON_FAC<=DBL_EPSILON){
    std::cout << "SDIRK: Error in constructor.\nEps must be " << 1/DEF_NEWTON_FAC;
    std::cout << " times larger than the unit roundoff " << DBL_EPSILON << "\n";
    exit(0);
  }
  // An instance of the SdirkNewtonRaphson class
  Newton = new SdirkNewtonRaphson(this, accur, num_ode, fun, jac);
  if (Newton==NULL)
    ErrorHandler(SE_ALLOC);
  // Select Butcher Tableau
  SelectMethod(methodType, Method);
  // Placeholders for coefficients
  OldK.Dimension(1, Method.stage, 1, num_ode);
  K.Dimension(1, Method.stage, 1, num_ode);
  
  switch(ctrl){
  case SC_ORDINARE:
    StepControl = new OrdStepControl(accur, Method.order, num_ode);
    break;
  case SC_PI:
    StepControl = new PiStepControl(accur, Method.order, num_ode);
    break;
  case SC_PRIM:
    StepControl = new PrimStepControl(accur, Method.order, num_ode);
    break;
  case SC_WATTS:
    StepControl = new WattsStepControl(accur, Method.order, num_ode);
    break;
  }
  if(StepControl == NULL)
    ErrorHandler(SE_ALLOC);
  Reset();
  // std::cout << "SDIRK: Instance declared succesfully\n";
}

// Destructor
Sdirk::~Sdirk(){
  delete Newton;
  delete StepControl;
  // std::cout << "SDIRK: Instance terminated succesfully\n";
}

// Private methods
// Calculate estimated error
void Sdirk::TruncError()
{
  int i, j;
  double temp;
  
  for(i = 1; i <= N; i++) {
    temp = 0.0;
    for(j = 1; j <= Method.stage; j++)
      temp += Method.d[j]*K(j, i);
    Error[i] = StepH*temp ;
  }
}

// Calculate the next solution
void Sdirk::Solution(DVector &y_out)
{
  int i, j;
  double temp;
  
  for(i = 1; i <= N; i++) {
    temp = 0.0;
    for(j = 1; j <= Method.stage; j++)
      temp += Method.b[j]*K(j, i);
    y_out[i] = StepY[i] + StepH*temp;
  }
}

// Set Pi
void Sdirk::SetPiConstant(PiCoefType &ok, PiCoefType &reject)
{
  StepControl->SetPiConstant(ok, reject);
}

// Select Butcher Tableau
void Sdirk::SelectMethod(SDIRKMethodType m, RKcoef &p)
{
    switch (m){
        case NT1:
            // Nielsen-Thomsen NT1
            p.stage = 4;
            p.order = 4;
            p.a.Dimension(1, 4, 1, 4);
            p.b.Dimension(1, 4);
            p.c.Dimension(1, 4);
            p.d.Dimension(1, 4);

            p.a(1, 1) = 0.0;
            p.a(2, 1) = 5.0/12.0;    p.a(2, 2) = 5.0/12.0;
            p.a(3, 1) = 95.0/588.0;  p.a(3, 2) = -5.0/49.0;   p.a(3, 3) = 5.0/12.0;
            p.a(4, 1) = 59.0/600.0;  p.a(4, 2) = -31.0/75.0;  p.a(4, 3) = 539.0/600.0;
            p.a(4, 4) = 5.0/12.0;

            p.b[1] = 59.0/600.0;     p.b[2] = -31.0/75.0;     p .b[3] = 539.0/600.0;
            p.b[4] = 5.0/12.0;

            p.c[1] = 0.0;            p.c[2] = 5.0/6.0;        p.c[3] = 10.0/21.0;
            p.c[4] = 1.0;

            p.d[1] = -37.0/600.0;    p.d[2] = -37.0/75.0;     p.d[3] = 1813.0/6600.0;
            p.d[4] = 37.0/132.0;

            rktype = IMPLICIT;
            break;

        case JKT:
            // Jørgensen-Kristensen-Thomsen ESDIRK34
            p.stage = 4;
            p.order = 4;
            p.a.Dimension(1, 4, 1, 4);
            p.b.Dimension(1, 4);
            p.c.Dimension(1, 4);
            p.d.Dimension(1, 4);

            p.a(1,1) = 0.0;
            p.a(2,1) = 0.43586652150845899942;
            p.a(2,2) = 0.43586652150845899942;
            p.a(3,1) = 0.14073777472470619619;
            p.a(3,2) = -0.1083655513813208000;
            p.a(3,3) = 0.43586652150845899942;
            p.a(4,1) = 0.10239940061991099768;
            p.a(4,2) = -0.3768784522555561061;
            p.a(4,3) = 0.83861253012718610911;
            p.a(4,4) = 0.43586652150845899942;

            p.b[1] = 0.10239940061991099768;
            p.b[2] = -0.3768784522555561061;
            p.b[3] = 0.83861253012718610911;
            p.b[4] = 0.43586652150845899942;

            p.c[1] = 0.0;
            p.c[2] = 0.87173304301691799883;
            p.c[3] = 0.46823874485184439565;
            p.c[4] = 1.0;

            p.d[1] = -0.05462549724041393942;
            p.d[2] = -0.49420889362599495480;
            p.d[3] = 0.22193449973506464477;
            p.d[4] = 0.32689989113134424957;

            rktype = IMPLICIT;
            break;

        case RKF34:
            // The Runge-Kutta-Fehlberg method
            p.stage = 5;
            p.order = 4;
            p.a.Dimension(1, 5, 1, 5);
            p.b.Dimension(1, 5);
            p.c.Dimension(1, 5);
            p.d.Dimension(1, 5);

            p.a(1,1) = 0.0;
            p.a(2,1) = 2.0/7.0;
            p.a(2,2) = 0.0;
            p.a(3,1) = 77.0/900.0;
            p.a(3,2) = 343.0/900.0;
            p.a(3,3) = 0.0;
            p.a(4,1) = 805.0/1444.0;
            p.a(4,2) = -77175.0/54872.0;
            p.a(4,3) = 97125.0/54872.0;
            p.a(4,4) = 0.0;
            p.a(5,1) = 79.0/490.0;
            p.a(5,2) = 0.0;
            p.a(5,3) = 2175.0/3626.0;
            p.a(5,4) = 2166.0/9065.0;
            p.a(5,5) = 0.0;

            p.b[1] = 229.0/1470.0;
            p.b[2] = 0.0;
            p.b[3] = 1125.0/1813.0;
            p.b[4] = 13718.0/81585.0;
            p.b[5] = 1.0/18.0;

            p.c[1] = 0.0;
            p.c[2] = 2.0/7.0;
            p.c[3] = 7.0/15.0;
            p.c[4] = 35.0/38.0;
            p.c[5] = 1.0;

            p.d[1] = -888.0/163170.0;
            p.d[2] = 0.0;
            p.d[3] = 3375.0/163170.0;
            p.d[4] = 11552.0/163170.0;
            p.d[5] = 9065.0/163170.0;

            rktype = EXPLICIT;
            break;

        default:
            p.stage = 4;
            p.order = 3;
            p.a.Dimension(1, 4, 1, 4);
            p.b.Dimension(1, 4);
            p.c.Dimension(1, 4);
            p.d.Dimension(1, 4);

            p.a(1, 1) = 0.0;
            p.a(2, 1) = 5.0/12.0;    p.a(2, 2) = 5.0/12.0;
            p.a(3, 1) = 95.0/588.0;  p.a(3, 2) = -5.0/49.0;   p.a(3, 3) = 5.0/12.0;
            p.a(4, 1) = 59.0/600.0;  p.a(4, 2) = -31.0/75.0;  p.a(4, 3) = 539.0/600.0;
            p.a(4, 4) = 5.0/12.0;

            p.b[1] = 59.0/600.0;     p.b[2] = -31.0/75.0;     p .b[3] = 539.0/600.0;
            p.b[4] = 5.0/12.0;

            p.c[1] = 0.0;            p.c[2] = 5.0/6.0;        p.c[3] = 10.0/21.0;
            p.c[4] = 1.0;

            p.d[1] = -37.0/600.0;    p.d[2] = -37.0/75.0;     p.d[3] = 1813.0/6600.0;
            p.d[4] = 37.0/132.0;

            rktype = IMPLICIT;
            break;
    }
  
}



// Public methods
 
// Reset the integrator to enable a restart
void Sdirk::Reset()
{
  FirstStep = TRUE;
  Info.NumOfGoodStep = 0;
  Info.NumOfBadStep = 0;
  Info.NumOfNewtonDivergens = 0;
  StepControl->Reset();
}

// Set the desired tolerance
void Sdirk::SetEps(double eps)
{
  StepControl->SetEps(eps);
};

// Get information on the recent integration
void Sdirk::GetInfo(SdirkInfoType &p)
{
  p.NumOfGoodStep        = Info.NumOfGoodStep;
  p.NumOfBadStep         = Info.NumOfBadStep;
  p.NumOfNewtonDivergens = Info.NumOfNewtonDivergens;
  p.MaxError = StepControl->GetMaxError();
}

// Show information on the recent integration
void Sdirk::ShowInfo()
{
  SdirkInfoType p;
  GetInfo(p); 
  std::cout << "SDIRK: Accepted steps  : " << p.NumOfGoodStep << "\n";
  std::cout << "SDIRK: Rejected steps  : " << p.NumOfBadStep << "\n";
  std::cout << "SDIRK: Divergent steps : " << p.NumOfNewtonDivergens << "\n";
  std::cout << "SDIRK: Maxsimal error  : " << p.MaxError << "\n";
}


// Perform integration from t=T[1] to t=T[2]
// For input, h holds the initial step length
// On return, h holds the latest step length
// For input, y holds y(T[1])
// On return, y holds y(T[2])
// For input, Nstep holds nothing significant
// On return, Nstep holds the number of used steps through the interval T
int Sdirk::Integrate(DVector &T, double &h, DVector &y, long &Nstep)
{
  double t, x = T[1];
  Nstep = 0;
  int errCode = GSL_SUCCESS;

  FirstStep = TRUE;

  // Reset the info
  Info.NumOfGoodStep = 0;
  Info.NumOfBadStep = 0;
  Info.NumOfNewtonDivergens = 0;

  // Call Integrate until T[2] is reached
  t = T[1];
  do {
    errCode = Integrate(t, h, y);
    if (errCode != GSL_SUCCESS)
        return errCode;
    x += h;
    t = T[2];
    Nstep++;
  } while(x<t);

  return errCode;
}

// Perform a single integration step
// First time, the function is called, t _must_ be lower integration limit
// Hereafter, t _must_ be upper integration limit
// 
// For input, h is the wanted step length
// On return, h is the actual used step length
// For input, y holds y(t)
// On return, y holds y(t+h)
int Sdirk::Integrate(double &t, double &h, DVector &y)
{
  int i, j, num_iteration, stage;
  BOOL step_ok, divergens;
  DVector y_temp(N), k_temp(N);
  double it_speed;
  int errCode = GSL_SUCCESS;
  
  StepY = y;

  if(FirstStep){
    StepH = h;
    StepX = t;
    errCode = UserF(StepX, y, k_temp);
    if (errCode != GSL_SUCCESS)
        return errCode;
    for(i=1; i<=N; i++)
      K(1,i) = k_temp[i];
  }
  else{
    for (i=1; i<=N; i++)
      K(1,i) = OldK(Method.stage,i);
    StepX += StepH;
    if (StepX+NextH-t>0.0)
      NextH = t - StepX;
    StepH = NextH;
  }
  
  step_ok = FALSE;

  while(!step_ok){
    stage = 2;
    divergens = FALSE;
    while(stage <= Method.stage && !divergens){
      
      if (rktype == IMPLICIT){
          // Newton->Iterate returns -1 in case of divergens
          // or else the number of iteration
          errCode = Newton->Iterate(stage, y_temp, it_speed, num_iteration);
      } else {
          // Explicit iteration
          errCode = ApplyStage(stage, y_temp);
          num_iteration = 1;
      }
      if (errCode != GSL_SUCCESS)
          return errCode;
      if (num_iteration<0){
	divergens = TRUE;
	Info.NumOfNewtonDivergens++;
      }
      else{
	errCode = UserF(StepX, y_temp, k_temp);
        if (errCode != GSL_SUCCESS)
            return errCode;
	for (i=1; i<=N; i++)
	  K(stage,i) = k_temp[i];
	stage++;
      }
    }

    if(!divergens)
      TruncError(); // Calculate estimated truncation error
    
    NextH = StepH;

    // StepControl->NewStepLength returns FALSE if the estimate
    // for the truncation error is large
    step_ok = StepControl->NewStepLength(Error,divergens,it_speed,NextH);
    if (StepH+NextH==StepH)
      ErrorHandler(SE_STEP_TO_SMALL);
    if(!step_ok){
      StepH = NextH;
      if(!divergens)
	Info.NumOfBadStep++;
    }
  }
  FirstStep = FALSE;
  OldK = K;
  Solution(y);
  h = StepH;
  Info.NumOfGoodStep++;

  return errCode;
}

int Sdirk::ApplyStage(int stage, DVector &y_out){
    double temp,x;

    for (int i=1; i<=N; i++ ){
        temp = 0.0;
        y_out[i] = StepY[i];
        for (int j=1; j<stage ; j++){
            y_out[i] += Method.c[stage] * StepH * Method.a(stage,j) * K(j,i);
        }
    }

    return GSL_SUCCESS;
}
