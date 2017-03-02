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
 * Author: Daniele Bigoni
 * E-mail: dabi@dtu.dk
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "string.h"

using namespace std;

void doubleMatrixAlloc(double** &mat, int nRows, int nCols){
  // dynamic allocation of 2-D arrays
  mat = new double*[nRows];
  for(int i = 0; i<nRows; i++)
    mat[i] = new double[nCols];
}

void doubleMatrixAllocSetZero(double** &mat, int nRows, int nCols){
    doubleMatrixAlloc(mat, nRows, nCols);
    for( int i = 0; i < nRows; i++ ){
        for( int j = 0; j < nCols; j++ ){
            mat[i][j] = 0.0;
        }
    }
}

void double3DMatrixAlloc(double*** &mat, int x, int y, int z){
  // dynamic allocation of 3-D arrays
  int ix;
  int iy;

  mat = new double**[x];
  for(ix=0;ix < x;ix++){
    mat[ix] = new double*[y];
    for (iy=0; iy<y; iy++)
      mat[ix][iy] = new double[z];
  }
}

void double3DMatrixCpy(double*** &out, double*** in, int x, int y, int z){
  // dynamic copy of 3-D arrays
  int ix;
  int iy;
  out = new double**[x];
  for(ix=0;ix < x;ix++){
    out[ix] = new double*[y];
    for (iy=0; iy<y; iy++){
      out[ix][iy] = new double[z];
      memcpy(out[ix][iy],in[ix][iy],z*sizeof(double));
    }
  }
}

/**
 * Non Standard sign function, but needed as it is.
 * @param val
 * @return
 */
double sign(double val){
    if ((val == 0.0) || (val == -0.0))
        return 1.0;
    else if (val > 0.0)
        return 1.0;
    else
        return -1.0;
}
