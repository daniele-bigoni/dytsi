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
 * E-mail: dabi@limitcycle.it
 *
 */

#ifndef TRACKCOMPONENT_H
#define TRACKCOMPONENT_H

#include "enumDeclarations.h"
#include "InterpolationFunction.h"

#include <stdio.h>
#include <string>

#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>

namespace DYTSI_Modelling {
  class ExternalComponents;

  class TrackComponent : public Component, public InterpolationFunction{
  public:
    enum ttable_values {
      TRK_L_lat, TRK_R_lat, TRK_L_vert, TRK_R_vert
    };

    TrackComponent(InterpolationTypes interpType = INTP_CUBICSPLINE,
                   gsl_vector* centerOfGeometry,
                   ExternalComponents* externalComponents,
                   std::string name,
                   GeneralModel* gm);
    TrackComponent(std::string path, InterpolationTypes interpType = INTP_CUBICSPLINE,
                   gsl_vector* centerOfGeometry,
                   ExternalComponents* externalComponents,
                   std::string name,
                   GeneralModel* gm);
    TrackComponent(double** table, int n_rows, int n_cols, 
                   InterpolationTypes interpType = INTP_CUBICSPLINE,
                   gsl_vector* centerOfGeometry,
                   ExternalComponents* externalComponents,
                   std::string name,
                   GeneralModel* gm);
    virtual ~TrackComponent();

    ////////////////////////////////////
    // Interpolation variables/functions
    double getValue(ttable_values col, double x);

    int get_N_COLS_TTABLE(){ return N_COLS_TTABLE; }
    int get_N_ROWS_TTABLE(){ return N_ROWS_TTABLE; }

    ////////////////////////////////
    // Component variables/functions
    static const int N_DOF = 4;

    // Indices of dof
    static const int Q_L_lat = 0; // Left lateral displacement
    static const int Q_R_lat = 0;
    static const int Q_L_vert = 0;
    static const int Q_R_vert = 0;
    static const sdt::string* DOF_Names[N_DOF];
    
    // Implementation of virtual Component classes
    int computeFun( double t, double y[], double f[] );
    int computeJac( double t, double y[], double* dfdy, double dfdt[]);

    int get_N_DOF() { return N_DOF; }
    
    std::vector<std::string> getHeader();
    void getStatus(double t, double* y, double* status);
    const double* getCurrentY();
    void init();
    std::string printCounters();
        
  private:
    ////////////////////////////////////
    // Interpolation variables/functions
    bool loaded;
    double** table;
    int N_COLS_TTABLE;
    int N_ROWS_TTABLE;
    gsl_interp_accel** accs;
    gsl_interp** interps;

    void readTrackTable(std::string path);
    void interpolate();

    ////////////////////////////////
    // Component variables/functions
    typedef Component super;
    
    enum TrackSide {LEFT,RIGHT}

    ComponentPosition position;

  protected:
    int computeConnectorsForcesAndMoments();
    int rhs(double t, const double y[], gsl_matrix* rightHandSide); 
  }
}

#endif /* TRACKCOMPONENT_H */
