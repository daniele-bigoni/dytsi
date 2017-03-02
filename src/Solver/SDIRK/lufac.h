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
 * Include file for linear system solver class    *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#ifndef __lufac_h
#define __lufac_h

#include "dmatrix.h"
#include "dvector.h"
#include "ivector.h"

// Solves a linear system
class LUfactorize
{
private:
  // Size
  int n;
  // Permutation vector
  IVector index;
  // LHS matrix A
  DMatrix a;
  // RHS vector b
  DVector b;
public:
  // Constructor
  LUfactorize(int dim);
  // Destructor
  ~LUfactorize();
  // Decompose using Doolittles Method
  void Decompose(DMatrix &mat);
  // Solve wrt. vec using Doolittles Method
  void Solve(DVector &vec);
};
#endif
