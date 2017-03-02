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
 * Source file for SdirkNewtonRaphson class       *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#include "sdirknewt.h"

#include <gsl/gsl_errno.h>

// Constructor
SdirkNewtonRaphson::SdirkNewtonRaphson
(Sdirk *p, double accur,
 int dimen,
 int(*fun)(double t, DVector &y, DVector &f),
 int(*jac)(double t, DVector &y, DMatrix &jacobi))
  : NewtonRaphsonBase(accur,dimen, fun, jac)
{
  Integrator = p;
}

// Calculate Newton-Raphson matrix
int SdirkNewtonRaphson::NewtonMatrix(int stage, DVector &y)
{
  int i, j;
  double gamma, x;
  int errCode = GSL_SUCCESS;
  
  x = Integrator->StepX + Integrator->Method.c[stage] * Integrator->StepH;
  gamma = Integrator->Method.a(stage, stage);
  
  errCode = UserJacobi(Integrator->StepX, y, Jacobi);
  if (errCode != GSL_SUCCESS)
      return errCode;
  
  for (i=1; i<=N; i++){
    for (j=1; j<=N; j++){
      Jacobi(i, j) *= Integrator->StepH*gamma;
      if(j==i)
	Jacobi(i, j) = 1.0 - Jacobi(i, j);
      else
	Jacobi(i, j) *= -1.0;
    }
  }
  return errCode;
}

// Calculate an initial guess for the Newton-Raphson iterations
void SdirkNewtonRaphson::InitialGuess(int stage, DVector &guess)
{
  int i, j;
  double theta,  r2, r3, r4 ;
  
  theta = 1.0 + Integrator->Method.c[stage-1];
  r2 = theta*(theta*(-28.0/5.0*theta + 62.0/5.0) - 8.0);
  r3 = theta*(theta*(98.0/55.0*theta - 49.0/10.0) + 49.9/11.0);
  r4 = theta*(theta*(42.0/11.0*theta -15.0/2.0) + 50.0/11.0);
  for (i=1; i<=N; i++) {
    guess[i] = Integrator->OldK(2, i)*r2 + Integrator->OldK(3, i)*r3;
    guess[i] += Integrator->OldK(4, i)*r4;
    guess[i] = Integrator->StepY[i] + Integrator->StepH*guess[i];
  }
}

// Calculate the Newton Function
int SdirkNewtonRaphson::NewtonFunction(DVector &y_in,
					int stage, DVector &g_out)
{
  int i, j;
  double temp, a, x;
  double gamma = Integrator->Method.a(stage, stage);
  int errCode = GSL_SUCCESS;
  
  x = Integrator->StepX + Integrator->Method.c[stage] * Integrator->StepH;
  errCode = UserF(x, y_in, g_out);
  if (errCode != GSL_SUCCESS)
      return errCode;

  for (i=1; i<=N; i++){
    temp = 0.0;
    for (j=1; j<=stage-1; j++)
      temp += Integrator->Method.a(stage, j) * Integrator->K(j, i);
    temp = Integrator->StepY[i]+Integrator->StepH*temp;
    a =  temp + gamma * Integrator->StepH * g_out[i];
    g_out[i] = - (y_in[i]-a);
  }
  return errCode;
}

// Perform Newton-Raphson iteration
int SdirkNewtonRaphson::Iterate(int stage, DVector &y_out, double &it_speed, int &div)
{
  int i, j ;
  BOOL divergens;
  double roc, new_norm, old_norm, c, sig;
  int p;
  DVector y_try(N);
  int errCode = GSL_SUCCESS;

  // In first step, no initial guess is given
  if(Integrator->FirstStep){
    y_out = Integrator->StepY;
    LastH = Integrator->StepH;
  }
  else{
    InitialGuess(stage, y_out);
    ThisH = Integrator->StepH;
  }

  // We avoid to calculate the Jacobian if the step length is unchanged
  if(stage == 2 && ThisH != LastH){
    errCode = NewtonMatrix(stage, y_out);
    if (errCode != GSL_SUCCESS)
        return errCode;
    LinSolver->Decompose(Jacobi);
  }

  divergens = FALSE;  

  // To avoid it_speed to be undefined in case of convergens in 
  // first iteration already, it_speed is set to the machine number
  it_speed = DBL_EPSILON;

  // First iteration
  i = 1;
  errCode = NewtonFunction(y_out, stage, y_try);
  if (errCode != GSL_SUCCESS)
      return errCode;
  LinSolver->Solve(y_try);
  y_out += y_try;
  old_norm = y_try.Norm(NT_TWO);
  // Second iteration (if necessary)
  if(old_norm > DEF_NEWTON_FAC*Eps){
    i = 2;
    errCode = NewtonFunction(y_out, stage, y_try);
    if (errCode != GSL_SUCCESS)
        return errCode;
    LinSolver->Solve(y_try);
    y_out += y_try;
    new_norm = y_try.Norm(NT_TWO);
    c = old_norm/(DEF_NEWTON_FAC*Eps);
    roc = new_norm / old_norm;
    p = (int) floor(1.0/log(roc)*log(1.0/c) + 0.5);
    sig = exp(1.0/DEF_MAX_ITERATIONS * log(1.0/c));
    
    if (roc>sig || roc>DEF_DIVERGENS_ROC){
      divergens = TRUE;
      it_speed = roc;
    }
    // Further iterations
    while (divergens!=TRUE && new_norm>DEF_NEWTON_FAC*Eps){
      errCode = NewtonFunction(y_out, stage, y_try);
      if (errCode != GSL_SUCCESS)
          return errCode;
      LinSolver->Solve(y_try);
      y_out += y_try;
      new_norm = y_try.Norm(NT_TWO);
      roc = new_norm / old_norm;
      it_speed = roc;
      if (roc>DEF_NEW_JACOBI_ROC && roc<DEF_DIVERGENS_ROC){
	errCode = NewtonMatrix(stage, y_out);
        if (errCode != GSL_SUCCESS)
            return errCode;
	LinSolver->Decompose(Jacobi);
      }
      i++;
      if (roc>DEF_DIVERGENS_ROC || i>DEF_MAX_ITERATIONS)
	divergens = TRUE;
      old_norm = new_norm;
    }
  }
    LastH = ThisH;

    if (divergens)
        div = -1;
    else
        div = i;
    
  return errCode;
}

