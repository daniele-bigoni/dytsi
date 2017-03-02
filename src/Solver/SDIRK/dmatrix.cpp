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
 * Source file for double matrix class            *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#include "dmatrix.h"

// Constructors
// Specifying index limits
DMatrix::DMatrix(int x_lo, int x_hi, int y_lo, int y_hi){
  x1 = x_lo;
  x2 = x_hi;
  y1 = y_lo;
  y2 = y_hi;
  Dimension(x1, x2, y1, y2);
  DimOk = TRUE;
}

// Specifying dimension
DMatrix::DMatrix(int n, int m){
  x1 = 1;
  x2 = n;
  y1 = 1;
  y2 = m;
  Dimension(x1, x2, y1, y2);
  DimOk = TRUE;
}

// Empty matrix
DMatrix::DMatrix(){
  x1 = x2 = y1 = y2 = 0;
  DimOk = FALSE;
}

// Copy constructor
// Needed, since the = operator is overloaded
DMatrix::DMatrix(const DMatrix & m)
{
  x1 = m.x1;
  x2 = m.x2;
  y1 = m.y1;
  y2 = m.y2;
  
  // Allocate memory 
  unsigned size = x2 - x1 + 1;
  mat = new double* [size];
  if(mat == NULL)
    ErrorHandler(M_ALLOC);
  size = y2 - y1 + 1;
  for(int i=0; i<=x2-x1; i++){
    mat[i] = new double[size];
    if(mat[i] == NULL)
      ErrorHandler(M_ALLOC);
  }

  // Copy values
  for(int i = 0; i <= x2-x1; i++)
    for(int j = 0; j <= y2-y1; j++)
      mat[i][j] = m.mat[i][j];
  DimOk = TRUE;
}

// Allocate memory to empty matrix (normal init)
void DMatrix::Dimension(int x_lo, int x_hi, int y_lo, int y_hi){
  x1 = x_lo;
  x2 = x_hi;
  y1 = y_lo;
  y2 = y_hi;
  DimOk = TRUE;
  
  // Allocate memory
  unsigned size = x2 - x1 + 1;
  mat = new double* [size];
  if(mat == NULL)
    ErrorHandler(M_ALLOC);
  size = y2 - y1 + 1;
  for(int i=0; i<=x2-x1; i++){
    mat[i] = new double[size];
    if(mat[i] == NULL)
      ErrorHandler(M_ALLOC);
  }

  // Initial value: Zero
  for(int i = 0; i <= x2-x1; i++)
    for(int j = 0; j <= y2-y1; j++)
      mat[i][j] = 0.;
}

// Destructor
DMatrix::~DMatrix(){
  if(DimOk){
    for(int i=x2-x1 ; i>=0; i--)
      delete mat[i];
    delete mat;
  }
}

// Index operator
double& DMatrix::operator ()(int index_x, int index_y){
  if( (index_x<x1) || (index_x>x2) || (index_y<y1) || (index_y>y2))
    ErrorHandler(M_RANGE);
  return( (double&) mat[index_x - x1][index_y - y1]);
}

// = method
DMatrix& DMatrix::operator = (const DMatrix& m)
{
  if(x1 != m.x1 || x2 != m.x2 || y1 != m.y1 || y2 != m.y2)
    ErrorHandler(M_NOT_IDENT);

  for(int i = 0; i <= x2-x1; i++)
    for(int j = 0; j <= y2-y1; j++)
      mat[i][j] = m.mat[i][j];
  return *this;
}

// += method
DMatrix& DMatrix::operator += (const DMatrix& m)
{
  if(x1 != m.x1 || x2 != m.x2 || y1 != m.y1 || y2 != m.y2)
    ErrorHandler(M_NOT_IDENT);
  
  for(int i=0; i<=x2-x1; i++)
    for(int j=0; j<=y2-y1; j++)
      mat[i][j] += m.mat[i][j];
  return *this;
}

// -= method
DMatrix& DMatrix::operator -= (const DMatrix& m)
{
  if(x1 != m.x1 || x2 != m.x2 || y1 != m.y1 || y2 != m.y2)
    ErrorHandler(M_NOT_IDENT);

  for(int i = 0; i <= x2-x1; i++)
    for(int j = 0; j <= y2-y1; j++)
      mat[i][j] -= m.mat[i][j];
  return *this;
}

// + method
DMatrix DMatrix::operator+(const DMatrix& m)
{  
  if(x1 != m.x1 || x2 != m.x2 || y1 != m.y1 || y2 != m.y2)
    ErrorHandler(M_NOT_IDENT);
  DMatrix temp(m.x1, m.x2, m.y1, m.y2);

  for(int i=0; i<=(m.x2-m.x1); i++)
    for(int j=0; j<=(m.y2-m.y1); j++)
      temp.mat[i][j] = m.mat[i][j] + mat[i][j];
  return temp;
}

// - method
DMatrix DMatrix::operator-(const DMatrix& m)
{  
  if(x1 != m.x1 || x2 != m.x2 || y1 != m.y1 || y2 != m.y2)
    ErrorHandler(M_NOT_IDENT);
  DMatrix temp(m.x1, m.x2, m.y1, m.y2);

  for(int i=0; i<=(m.x2-m.x1); i++)
    for(int j=0; j<=(m.y2-m.y1); j++)
      temp.mat[i][j] = m.mat[i][j] - mat[i][j];
  return temp;
}

// Prototype for default error handler
static void DefaultHandler(MatError err);

void (*(DMatrix::ErrorHandler))(MatError) = DefaultHandler;

static void DefaultHandler(MatError err){
  std::cerr << "SDIRK : Error in DMatrix object: ";
  switch(err){
  case M_ALLOC :
    std::cerr << "Memory allocation failure";
    break;
  case M_RANGE:
    std::cerr << "Index out of range";
    break;
  case M_NOTDIM:
    std::cerr << "Matrix range are not known. Use Dimension";
    break;
  case M_NOT_IDENT:
    std::cerr << "Left and right DMatrix must be of same dimension when \n"
	 << "operator =, +=, -=, + and - are used";
    break;
  }
  std::cerr << "\n";
  exit(0);
}

// Flush to ostream
std::ostream& operator << (std::ostream& os, const DMatrix& m)
{
  for (int i=m.x1; i<=m.x2; i++)
    for (int j=m.y1; j<=m.y2; j++){
      os << "(" << i << "," << j << ")=" << m.mat[i-1][j-1] << "   ";
      if (j==m.y2)
	os << "\n";
    }
  return os;
}

