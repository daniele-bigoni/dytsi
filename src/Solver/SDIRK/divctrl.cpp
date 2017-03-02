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
 * Source file for Divergens Step Control class   *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#include "divctrl.h"

// Calculate the new steplength
void DivergensStepControl::NewStepLength(BOOL newton_divergens,
					 double roc, double &h)
{
  double h_roc;
  
  if(newton_divergens){
    if (roc>DEF_NEW_JACOBI_ROC)
      h_roc = h / (roc/DEF_NEW_JACOBI_ROC);
    else
      h_roc = 0.7*h;
  }
  else
    h_roc = h / (roc/DEF_NEW_JACOBI_ROC);
  h = DEF_SAFE_FAC*h_roc;
}
