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
 * Include file for integer vector class          *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#ifndef __ivector_h
#define __ivector_h

#include "types.h"

class IVector
{
private:
  // Upper and lower limits for index
  int upper, lower;
  // Pointer to the elements of the vector
  int *vec;
  // Boolean indicating wheather the dimension of the vector is okay or not
  BOOL DimOk;
  // Pointer to errorhandler
  static void (*ErrorHandler)(VecError err);
public:
  // Constructors
  IVector(int lo, int hi);
  IVector(int size);
  IVector();
  // Copy constructor
  IVector(const IVector& v);
  // Destructor
  ~IVector();
  // Set the dimension. Initialize to the zero vector
  void Dimension(int lo, int hi);
  // Calculate the norm
  double Norm(NormType p);
  // Set the specified error handler
  static void SetErrorHandler(void(*user_handler)(VecError err));
  // [] operator overloaded as matrix subscript operator
  int& operator [] (int index);
  // Assignment methods
  IVector& operator =  (const IVector& v);
  IVector& operator += (const IVector& v);
  IVector& operator -= (const IVector& v);
  // Binary methods
  IVector operator + (const IVector& v);
  IVector operator - (const IVector& v);
  // Flush to ostream
  friend std::ostream& operator << (std::ostream& os, const IVector& v);
};
#endif
