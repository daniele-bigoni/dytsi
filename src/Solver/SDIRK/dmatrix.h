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
 * Include file for double matrix class           *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#ifndef __dmatrix_h
#define __dmatrix_h

#include "types.h"

class DMatrix
{
private:
  // Matrix dimension:  (x2-x1+1) X (y2-y1+1)
  int x1, x2, y1, y2;
  // Pointer to the elements in the matrix
  double **mat;
  // Boolean indicating wheather the dimension of the vector is okay or not
  BOOL DimOk;
  // Error handler
  static void (*ErrorHandler)(MatError err);
public:
  // Constructors
  DMatrix(int x_lo, int x_hi, int y_lo, int y_hi);
  DMatrix(int n, int m);
  DMatrix();
  // Copy constructor
  DMatrix(const DMatrix& m);
  // Destructor
  ~DMatrix();
  // Set the dimension. Initialize to the zero matrix
  void Dimension(int x_lo, int x_hi, int y_lo, int y_hi);
  // Set the specified error handler
  static void SetErrorHandler(void(*user_handler)(MatError err));
  // () operator overloaded as matrix subscript operator
  double& operator ()(int index_x, int index_y);
  // Assignment methods
  DMatrix& operator =  (const DMatrix& m);
  DMatrix& operator += (const DMatrix& m);
  DMatrix& operator -= (const DMatrix& m);
  // Binary methods
  DMatrix operator + (const DMatrix& m);
  DMatrix operator - (const DMatrix& m);
  // Flush to ostream
  friend std::ostream& operator << (std::ostream& os, const DMatrix& M);
};
#endif
