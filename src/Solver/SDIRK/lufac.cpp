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
 * Source file for linear system solver class     *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#include "lufac.h"

#define dswap(a,b) {int temp=(a); (a)=(b); (b)=(temp); }

// Prototypes
int LU_decompose(DMatrix &a, IVector &index, int n);
int LU_solve(DMatrix &a, DVector &b, int n, IVector &index);

// Constructor
LUfactorize::LUfactorize(int dim)
{
  n = dim;
  index.Dimension(1,n);
  index = IVector(n);

  a.Dimension(1,n,1,n);
  a = DMatrix(n, n);

  b.Dimension(1,n);
  b = DVector(n);
}

// Destructor
LUfactorize::~LUfactorize(){}

// Decompose mat
// On return, mat contains both upper and lower triangular
// from Doolittles method
void LUfactorize::Decompose(DMatrix &mat)
{
  a = mat;
  int cond = LU_decompose(a, index, n);
  if (cond==SINGUL){
    std::cout << "SDIRK: Warning in LUfactorize::Decompose: Matrix singular\n";
    exit(0);
  }
  else
    mat = a;
}

// Solve with respect to vec.
// On return, vec contains the solution
void LUfactorize::Solve(DVector &vec)
{
  b = vec;
  int cond = LU_solve(a, b, n, index);
  vec = b;
}

/* LU_decompose
 * Decompose the matrix a using Doolittles Pivot Method.
 * The output is stored in a.
 * The function returns SINGUL, if a is singular.
 * The function returns OK, if calculations are okay.
 */
int LU_decompose(DMatrix &a, IVector &index, int n)
{
  int i, j, k, swapindex, ii, jj, kk;
  double maxsofar, p_sum, a_ii;

  /* Initialize the permutation index vector */
  for (i=1; i<=n; i++) index[i] = i;

  /* Loop for all rows */
  for (i=1; i<= n; i++){
    ii = index[i];
    /* Calculate upper triangle */
    for (j=1; j<i;  j++){
      jj = index[j];
      p_sum = a(jj,i);
      for (k=1; k<j; k++){
	kk = index[k];
	p_sum -= a(jj,k)*a(kk,i);
      }
      a(jj,i) = p_sum;
    }
    maxsofar = 0.0;
    swapindex = i;

    /* Calculate lower triangle */
    for (j=i; j<=n; j++){
      jj = index[j];
      p_sum = a(jj,i);
      for (k=1; k<i; k++){
	kk = index[k];
	p_sum -= a(jj,k)*a(kk,i);
      }
      a(jj,i) = p_sum;
      /* Locate Pivot Element */
      if (fabs(p_sum)>maxsofar){
	maxsofar = fabs(p_sum);
	swapindex = j;
      }
    }

    if (maxsofar<DBL_EPSILON) return SINGUL;

    if (swapindex!=i){
      dswap(index[i], index[swapindex]);
      ii = index[i];
    }

    if (i!=n){
      a_ii = a(ii,i);
      for (j=i+1; j<=n; j++){
	jj = index[j];
	a(jj,i) = a(jj,i)/a_ii;
      }
    }
  } /* We're through all rows */
  return OK;
}

/* LU_solve
 * Solves a linear system, which is decomposed using LU_decompose.
 * The output is the solution vector.
 */
int LU_solve(DMatrix &a, DVector &b, int n, IVector &index)
{
  int i, j, ii, jj;
  double p_sum;
  DVector temp(n);

  /* Forward substitution */
  for (i=1; i<=n; i++) {
    ii = index[i];
    p_sum = b[ii];
    for (j=1; j<=i-1; j++) {
      jj = index[j];
      p_sum -= a(ii,j)*b[jj];
    }
    b[ii] = p_sum;
  }

  /* Backward substitution */
  for (i=n; i>=1; i--) {
    ii = index[i];
    p_sum = b[ii];
    for (j=i+1; j<=n; j++) {
      jj = index[j];
      p_sum -= a(ii,j)*b[jj];
    }
    b[ii] = p_sum/a(ii,i);
  }

  /* Update solution taking in account the possible permutation */
  for (i=1; i<=n; i++)
    temp[i] = b[index[i]];
  for (i=1; i<=n; i++)
    b[i] = temp[i];

  return OK;
}
