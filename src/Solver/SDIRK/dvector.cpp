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
 * Source file for double vector class            *
 * Implemented in MS-DOS 1991 by Michael Jeppesen *
 * Ported to UNIX spring 1998 by Erik Ostergaard  *
 **************************************************/

#include "dvector.h"

#define max(a, b)  (((a) > (b)) ? (a) : (b) )

// Constructors
// Specifying index limits
DVector::DVector(int lo, int hi){
  Dimension(lo, hi);
  DimOk = TRUE;
}
// Specifying size
DVector::DVector(int size){
  Dimension(1, size);
  DimOk = TRUE;
}
// Unspecified index limits
DVector::DVector(){
  upper = lower = 0;
  DimOk = FALSE;
}

// Copy constructor
// Needed, since the = operator is overloaded
DVector::DVector(const DVector & v)
{
  upper = v.upper;
  lower = v.lower;
  unsigned size = upper - lower + 1;
  vec = new double[size];
  for (int i=0; i<size; i++)
    vec[i]=0.;
  if (vec == NULL)
    ErrorHandler(V_ALLOC);

  // Copy values
  for(int i=0; i<size; i++)
    vec[i] = v.vec[i];
  DimOk = TRUE;
}

// Destructor
DVector::~DVector(){
  if (DimOk) delete vec;
}

// Initializing the vector. Alloating memory.
void DVector::Dimension(int lo, int hi){
  unsigned size;
  upper = hi;
  lower = lo;
  DimOk = TRUE;
  size = upper - lower + 1;
  vec = new double[size];
  for (int i=0; i<size; i++)
    vec[i]=0.;
}

// Calculate the norm of the vector
double DVector::Norm(NormType p){
  int i;
  double temp = 0.0;
  switch (p){
  case NT_ONE: // one norm (sum of all elements)
    for(i=lower; i<=upper; i++)
      temp += fabs(vec[i-lower]);
    break;
  case NT_TWO: // two norm (sqrt of sum of squared elements))
    for(i=lower; i<=upper; i++)
      temp += vec[i-lower]*vec[i-lower];
    temp = sqrt(temp);
    break;
  case NT_INF: // infinity norm (largest element)
    for(i=lower; i<=upper; i++)
      temp = fabs (max(temp, fabs(vec[i-lower])));
    break;
  }
  return(temp);
}

// New operator [] returning the demanded element
double& DVector::operator [] (int index){
  if((index<lower) || (index>upper))
    ErrorHandler(V_RANGE);
  return( (double&) vec[index - lower]);
}

// = method
DVector& DVector::operator = (const DVector& v)
{
  if(upper!=v.upper || lower!=v.lower)
    ErrorHandler(V_NOT_IDENT);
  for(int i=0; i<=upper-lower; i++)
    vec[i] = v.vec[i];
  return *this;
}

// += method
DVector& DVector::operator += (const DVector& v)
{
  if(upper!=v.upper || lower!=v.lower)
    ErrorHandler(V_NOT_IDENT);
  for(int i=0; i<=upper-lower; i++)
    vec[i] += v.vec[i];
  return *this;
}

// -= method
DVector& DVector::operator -= (const DVector& v)
{
  if(upper!=v.upper || lower!=v.lower)
    ErrorHandler(V_NOT_IDENT);
  for(int i=0; i<=upper-lower; i++)
    vec[i] -= v.vec[i];
  return *this;
}

// + method
DVector DVector::operator+(const DVector& v)
{  
  if(upper!=v.upper || lower!=v.lower)
    ErrorHandler(V_NOT_IDENT);
  DVector temp(v.lower, v.upper);
  for(int i=0; i<=(upper-lower); i++)
    temp.vec[i] = v.vec[i] + vec[i];
  return temp;
}

// - method
DVector DVector::operator-(const DVector& v)
{  
  if(upper!=v.upper || lower!=v.lower)
    ErrorHandler(V_NOT_IDENT);
  DVector temp(v.lower, v.upper);
  for(int i=0; i<=(upper-lower); i++)
    temp.vec[i] = v.vec[i] - vec[i];
  return temp;
}

// Prototype for default error handler
static void DefaultHandler(VecError err);
// Choosing error handler
void (*(DVector::ErrorHandler))(VecError) = DefaultHandler;
// Error handler:
static void DefaultHandler(VecError err){
  std::cerr << "SDIRK: Error in DVector object: ";
  switch(err)
    {
    case V_ALLOC :
      std::cerr << "Memory allocation failure";
      break;
    case V_RANGE:
      std::cerr << "Index out of range";
      break;    
    case V_NOT_IDENT:
      std::cerr << "Left and right DVector must be of same dimension when \n"
	   << "operator =, +=, -=, + and - are used";
      break;
    }
  std::cerr << "\n";
  exit(0);
}

// Flush to ostream
std::ostream& operator << (std::ostream& os, const DVector& v)
{
  for (int i=v.lower; i<=v.upper; i++)
    os << "[" << i << "]=" << v.vec[i-1] << "\n";
  return os;
}

void DVector::setVector(double* vec){
    delete[] this->vec;
    this->vec = vec;
}

double* DVector::getVector(){
    return this->vec;
}
