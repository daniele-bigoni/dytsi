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
 * File:   RSGEOTable.cpp
 * Author: s083781
 * 
 * Created on February 14, 2011, 2:57 PM
 */

#include "RSGEOTable.h"

#include "../Application/PrintingHandler.h"
#include "../Application/miscellaneous.h"

#include <sstream>
#include <fstream>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace DYTSI_Modelling {
    RSGEOTable::RSGEOTable(InterpolationTypes interpType)
    : InterpolationFunction(interpType) {
        this->loaded = false;
    }

    RSGEOTable::RSGEOTable(std::string path, InterpolationTypes interpType)
    : InterpolationFunction(interpType) {
        this->readRSGEO(path);
    }

    RSGEOTable::~RSGEOTable() {
        if (this->loaded) {
            for( int i = 1; i < N_COLS_RSGEO; i++ ){
                gsl_interp_free (interps[i]);
                gsl_interp_accel_free (accs[i]);
            }
        }
    }
    
    void RSGEOTable::readRSGEO(std::string pathS){
      struct stat stFileInfo;
      
      std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
      *out << "\t\t\tReading RSGEO file " << pathS << "\t" << std::flush;
      PrintingHandler::printOut(out,PrintingHandler::STDOUT);
      delete out;

      if (stat(pathS.c_str(),&stFileInfo)==0){
        // Count the lines
        std::ifstream rsgeofile(pathS.c_str());
        std::string line;
        int counter = 0;
        std::getline(rsgeofile,line);
        std::stringstream lineStream(line,std::stringstream::in);
        std::string entry;
        while( lineStream >> entry ) counter++;
        this->N_COLS_RSGEO = counter;
        counter = 0;
        while( std::getline(rsgeofile,line) ) counter++;
        this->N_ROWS_RSGEO = counter;
        rsgeofile.clear();
        rsgeofile.seekg(0);

        // Read the file
        doubleMatrixAlloc(this->table,N_COLS_RSGEO,N_ROWS_RSGEO);
        const int numchar = 400;
        char buffer[numchar];
        double val;

        rsgeofile.getline(buffer,numchar);
        for( int i = 0; i < N_ROWS_RSGEO; i++){
          for ( int j = 0; j < N_COLS_RSGEO; j++){
            rsgeofile >> val;
            this->table[j][i] = val;
            if ((i==0) && (j==0)) MAX_DISPL = fabs(val);
          }
        }
        rsgeofile.close();
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "[DONE]" << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;

        this->interpolate();

        this->loaded = true;
      } else {
        std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "[FAILED]" << std::endl;
        *out << "\t\t\tError: file " << pathS << " missing." << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        this->loaded = false;
      }
    }

    RSGEOTable::RSGEOTable(double** table, int n_rows, int n_cols, double max_displ, InterpolationTypes interpType)
    : InterpolationFunction(interpType)
    {
        this->N_COLS_RSGEO = n_cols;
        this->N_ROWS_RSGEO = n_rows;
        this->MAX_DISPL = max_displ;
        this->table = table;

        this->interpolate();

        this->loaded = true;
    }

    void RSGEOTable::interpolate(){
        // Perform interpolation
        std::stringstream* out;
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\t\tInterpolating RSGEO\t" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        accs = (gsl_interp_accel**)malloc(N_COLS_RSGEO*sizeof(gsl_interp_accel*));
        interps = (gsl_interp**)malloc(N_COLS_RSGEO*sizeof(gsl_interp*));
        for( int i = 1; i<N_COLS_RSGEO; i++ ){
          accs[i] = gsl_interp_accel_alloc ();
          interps[i] = this->allocInterp(this->N_ROWS_RSGEO);
          //interps[i] = gsl_interp_alloc (gsl_interp_cspline, N_ROWS_RSGEO);
          gsl_interp_init(interps[i], table[0], table[i], N_ROWS_RSGEO);
        }
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "[DONE]" << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
    }

    double RSGEOTable::getValue(rsgeo_values col, double x){
        return ( gsl_interp_eval(this->interps[col], this->table[0], this->table[col], x, this->accs[col]) );
    }

    double RSGEOTable::get_MAX_DISPL() {
        return this->MAX_DISPL;
    }
}
