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
 * File:   RSGEOTableReader.cpp
 * Author: s083781
 * 
 * Created on May 27, 2011, 12:27 PM
 */

#include "RSGEOTableReader.h"

#include "../PrintingHandler.h"
#include "../miscellaneous.h"

#include "../../GeneralModel/enumDeclarations.h"

#include <fstream>
#include <vector>
#include <sstream>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace DYTSI_Input{
    RSGEOTableReader::RSGEOTableReader() {
    }

    RSGEOTableReader::~RSGEOTableReader() {
    }

    void RSGEOTableReader::read(std::vector<DYTSI_Modelling::RSGEOTable*>& rsgList, std::string pathS, char tabDelimiter){
        struct stat stFileInfo;
        int N_COLS_RSGEO;
        int N_ROWS_RSGEO;
        double MAX_DISPL;

        std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\t\tReading RSGEO file " << pathS << "\n" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;

        if (stat(pathS.c_str(),&stFileInfo)==0){
            int counter = 0;
            int counterRSGEO = 0;
            int startingPosition;

            // Open the file
            std::ifstream rsgeofile(pathS.c_str());
            std::string line;
            while (std::getline(rsgeofile,line)) {
                counterRSGEO++;
                
                // Find the first delimiter
                char delim = (line.c_str())[0];
                while ( delim != tabDelimiter && std::getline(rsgeofile,line) ){
                    delim = (line.c_str())[0];
                }
                if (delim != tabDelimiter)
                    break;

                // Count the colums
                std::getline(rsgeofile,line);
                std::stringstream *lineStream = new std::stringstream(line,std::stringstream::in);
                std::string entry;
                counter = 0;
                while( *lineStream >> entry ) counter++;
                N_COLS_RSGEO = counter;
                startingPosition = rsgeofile.tellg();

                // Count the rows
                delim = ' ';
                counter = 0;
                while( (delim != tabDelimiter) && std::getline(rsgeofile,line) ){
                    delim = (line.c_str())[0];
                    counter++;
                }
                N_ROWS_RSGEO = counter-1;

                // Allocate the RSGEO TABLE
                double** rsgeo_table;
                doubleMatrixAlloc(rsgeo_table,N_COLS_RSGEO,N_ROWS_RSGEO);
                // Start reading
                rsgeofile.clear();
                rsgeofile.seekg(startingPosition);
                double val;
                for( int i = 0; i < N_ROWS_RSGEO; i++){
                  std::getline(rsgeofile,line);
                  lineStream = new std::stringstream(line,std::stringstream::in);
                  for ( int j = 0; j < N_COLS_RSGEO; j++){
                    *lineStream >> val;
                    rsgeo_table[j][i] = val;
                    if ((i==0) && (j==0)) MAX_DISPL = fabs(val);
                  }
                }
                DYTSI_Modelling::RSGEOTable* rsgeo = new DYTSI_Modelling::RSGEOTable(rsgeo_table,N_ROWS_RSGEO,N_COLS_RSGEO,MAX_DISPL);
                rsgList.push_back(rsgeo);
            }
        }
    }

    void RSGEOTableReader::read(std::vector<DYTSI_Modelling::RSGEOTable*>& rsgList, XERCES_CPP_NAMESPACE::DOMNode* node, char tabDelimiter){
        std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        
        // Read the path and the interpolation type
        std::string filePath = XMLaux::setValueToString(node);
        XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = node->getAttributes();
        XERCES_CPP_NAMESPACE::DOMAttr* interpAttribute =
                (XERCES_CPP_NAMESPACE::DOMAttr*)pAttributes->getNamedItem(
                XERCES_CPP_NAMESPACE::XMLString::transcode("Interp"));
        char* interpTypeStr = XERCES_CPP_NAMESPACE::XMLString::transcode(interpAttribute->getValue());
        DYTSI_Modelling::InterpolationTypes interpType;
        if (strcmp(interpTypeStr,"linear") == 0){
            interpType = DYTSI_Modelling::INTP_LINEAR;
        } else if (strcmp(interpTypeStr,"cspline") == 0) {
            interpType = DYTSI_Modelling::INTP_CUBICSPLINE;
        } else if (strcmp(interpTypeStr,"akima") == 0) {
            interpType = DYTSI_Modelling::INTP_AKIMA;
        } else {
            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << "\t\tThe interpolation type " << interpTypeStr << " doesn't exists." << std::endl;
            PrintingHandler::printOut(out,PrintingHandler::STDOUT);
            delete out;
            return;
        }

        // Read and interplate RSGEO tables
        struct stat stFileInfo;
        int N_COLS_RSGEO;
        int N_ROWS_RSGEO;
        double MAX_DISPL;

        *out << "\t\t\tReading RSGEO file " << filePath << "\n" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;

        if (stat(filePath.c_str(),&stFileInfo)==0){
            int counter = 0;
            int counterRSGEO = 0;
            int startingPosition;

            // Open the file
            std::ifstream rsgeofile(filePath.c_str());
            std::string line;
            while (std::getline(rsgeofile,line)) {
                counterRSGEO++;

                // Find the first delimiter
                char delim = (line.c_str())[0];
                while ( delim != tabDelimiter && std::getline(rsgeofile,line) ){
                    delim = (line.c_str())[0];
                }
                if (delim != tabDelimiter)
                    break;

                // Count the colums
                std::getline(rsgeofile,line);
                std::stringstream *lineStream = new std::stringstream(line,std::stringstream::in);
                std::string entry;
                counter = 0;
                while( *lineStream >> entry ) counter++;
                N_COLS_RSGEO = counter;
                startingPosition = rsgeofile.tellg();

                // Count the rows
                delim = ' ';
                counter = 0;
                while( (delim != tabDelimiter) && std::getline(rsgeofile,line) ){
                    delim = (line.c_str())[0];
                    counter++;
                }
                N_ROWS_RSGEO = counter-1;

                // Allocate the RSGEO TABLE
                double** rsgeo_table;
                doubleMatrixAlloc(rsgeo_table,N_COLS_RSGEO,N_ROWS_RSGEO);
                // Start reading
                rsgeofile.clear();
                rsgeofile.seekg(startingPosition);
                double val;
                for( int i = 0; i < N_ROWS_RSGEO; i++){
                  std::getline(rsgeofile,line);
                  lineStream = new std::stringstream(line,std::stringstream::in);
                  for ( int j = 0; j < N_COLS_RSGEO; j++){
                    *lineStream >> val;
                    rsgeo_table[j][i] = val;
                    if ((i==0) && (j==0)) MAX_DISPL = fabs(val);
                  }
                }
                DYTSI_Modelling::RSGEOTable* rsgeo = new DYTSI_Modelling::RSGEOTable(
                        rsgeo_table,N_ROWS_RSGEO,N_COLS_RSGEO,MAX_DISPL, interpType);
                rsgList.push_back(rsgeo);
            }
        }

    }

}
