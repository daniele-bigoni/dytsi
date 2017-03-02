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

/* 
 * File:   RSGEOTable.h
 * Author: s083781
 *
 * Created on February 14, 2011, 2:57 PM
 */

#ifndef RSGEOTABLE_H
#define	RSGEOTABLE_H

#include "enumDeclarations.h"
#include "InterpolationFunction.h"

#include <stdio.h>
#include <string>

#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>

namespace DYTSI_Modelling {
    class RSGEOTable : public InterpolationFunction {
    public:
        enum rsgeo_values {
            RSG_lat,RSG_N,RSG_angle,RSG_a,RSG_b,RSG_Kwy,RSG_Kwz,
            RSG_C11,RSG_C22,RSG_C23,RSG_Krz,RSG_qN,RSG_Kry};


        RSGEOTable(InterpolationTypes interpType = INTP_LINEAR);
        RSGEOTable(std::string path, InterpolationTypes interpType = INTP_LINEAR);
        RSGEOTable(double** table, int n_rows, int n_cols, double max_displ, InterpolationTypes interpType = INTP_LINEAR);
        virtual ~RSGEOTable();

        double getValue(rsgeo_values col, double x);

        int get_N_COLS_RSGEO(){ return N_COLS_RSGEO; }
        int get_N_ROWS_RSGEO(){ return N_ROWS_RSGEO; }

        double get_MAX_DISPL();
        
    private:
        bool loaded;
        double** table;
        int N_COLS_RSGEO;
        int N_ROWS_RSGEO;
        double MAX_DISPL;
        gsl_interp_accel** accs;
        gsl_interp** interps;

        // Methods
        void readRSGEO(std::string path);

        void interpolate();

    };
}

#endif	/* RSGEOTABLE_H */

