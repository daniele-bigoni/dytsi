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
 * File:   OutputHandler.cpp
 * Author: s083781
 * 
 * Created on March 16, 2011, 10:35 AM
 */

#include "OutputHandler.h"

#include "../Input/Input.h"
#include "../Input/XMLaux.h"
#include "../Input/InputHandler.h"
#include "../PrintingHandler.h"

#include <string>
#include <iosfwd>
#include <sstream>
#include <fstream>
#include <vector>

namespace DYTSI_Output {

    char TAB_SEPARATOR[] = "\t";
    char TAB_NEWENTRY[] = "\n";
    const char OUTPUT_FILE_NAME[] = "output";
    const char OUTPUT_FILE_EXT[] = ".dat";
    const char JACOBIAN_DIR_NAME[] = "Jacobians";
    const char JACOBIAN_FILE_NAME[] = "jac";
    const char JACOBIAN_FILE_EXT[] = ".dat";

    const std::string* OutputHandler::FormatTypeNames[N_FORMAT_TYPE] = { new std::string("TAB") };

    const std::string* OutputHandler::PlottingTypeNames[N_PLOTTING_TYPE] = { new std::string("NONE"), new std::string("GNUPlot") };

    OutputHandler::OutputHandler( DYTSI_Input::Input* input, XERCES_CPP_NAMESPACE::DOMElement* node, int& error) {
        std::string prop;
        bool found;

        // Set up the input
        this->input = input;

        // Set up the format
        error += XMLaux::loadStringProperty(node, "Format", prop);
        found = false;
        for (int i = 0; i < N_FORMAT_TYPE && !found; i++){
            if (prop.compare(*FormatTypeNames[i]) == 0){
                this->format = (FormatType)i;
                found = true;
            }
        }
        if (!found){
            error = 1;
            return;
        }

        // Set up the plotting
        prop.clear();
        error += XMLaux::loadStringProperty(node, "Format", prop);
        found = false;
        for (int i = 0; i < N_FORMAT_TYPE && !found; i++){
            if (prop.compare(*FormatTypeNames[i]) == 0){
                this->plotting = (PlottingType)i;
                found = true;
            }
        }
        if (!found){
            error = 2;
            return;
        }
    }

    OutputHandler::~OutputHandler() {
    }

    void OutputHandler::reset(){
        std::stringstream* path = new std::stringstream(std::stringstream::in | std::stringstream::out);
        *path << this->input->getInputHandler()->getOutputFolder() << "/";
        *path << this->input->getTestName() << "/";

        // Reset the solution file
        std::stringstream* filePath = new std::stringstream(std::stringstream::in | std::stringstream::out);
        *filePath << path->str() << OUTPUT_FILE_NAME << OUTPUT_FILE_EXT;

        this->solPathFile = filePath->str();
        std::fstream outFile(this->solPathFile.c_str(), std::ofstream::out);
        outFile.close();

        // Create the jacobian directory
        std::stringstream* jacFolder = new std::stringstream(std::stringstream::in | std::stringstream::out);
        *jacFolder << path->str() << JACOBIAN_DIR_NAME << "/";
        this->jacPathDir = jacFolder->str();
        int error = this->input->getInputHandler()->createOutputFolder(this->jacPathDir.c_str());
        if (error != 0) return;

        // Setting up separators
        switch (this->format){
            case TAB:
                this->valuesSeparator = TAB_SEPARATOR;
                this->entriesSeparator = TAB_NEWENTRY;
                break;
        }
    }

    void OutputHandler::writeSolutionHeader(std::vector<std::string> header){
        std::fstream outFile(this->solPathFile.c_str(), std::fstream::out|std::fstream::app);

        // Write the header
        for(std::vector<std::string>::iterator it = header.begin();
                it != header.end(); ++it){
            std::string headerEntry = *it;
            outFile << "\"" << headerEntry << "\"" << valuesSeparator;
        }
        outFile << entriesSeparator;

        outFile.close();
    }

    void OutputHandler::writeSolutionEntry(double* entry, int N_VARS){
        std::fstream outFile(this->solPathFile.c_str(), std::fstream::out|std::fstream::app);
        outFile.precision(10);

        // Write one entry
        for( int i = 0; i < N_VARS; i++ ){
            outFile << std::scientific << entry[i] << valuesSeparator;
        }
        outFile << entriesSeparator;

        outFile.close();
    }

    void OutputHandler::writeJacobian(double* dfdy, int N_VARS, int idx){
        std::stringstream* jacFile = new std::stringstream(std::stringstream::in | std::stringstream::out);
        *jacFile << this->jacPathDir << JACOBIAN_FILE_NAME << "-" << idx << JACOBIAN_FILE_EXT;

        std::fstream outFile(jacFile->str().c_str(), std::fstream::out|std::fstream::app);

        // Set the matrix view
        gsl_matrix_view dfdy_mat = gsl_matrix_view_array (dfdy,N_VARS,N_VARS);

        // Write
        for(int i = 0; i < N_VARS; i++){
            for( int j = 0; j < N_VARS; j++ ){
                outFile << gsl_matrix_get(&dfdy_mat.matrix, i, j) << "\t";
            }
            outFile << std::endl;
        }

        // Close the file
        outFile.close();
    }

//    void OutputHandler::writeSolution(DYTSI_Solver::Solution* solution){
//        std::stringstream* out;
//
//        char* valuesSeparator;
//        char* entriesSeparator;
//
//        switch (this->format){
//            case TAB:
//                valuesSeparator = TAB_SEPARATOR;
//                entriesSeparator = TAB_NEWENTRY;
//        }
//
//        // Open the file
//        std::stringstream* filePath = new std::stringstream(std::stringstream::in | std::stringstream::out);
//        *filePath << this->input->getInputHandler()->getOutputFolder() << "/";
//        *filePath << this->input->getTestName() << "/";
//        *filePath << OUTPUT_FILE_NAME;
//        std::ofstream outFile(filePath->str().c_str());
//
//        out = new std::stringstream(std::stringstream::in | std::stringstream::out);
//        *out << "Writing output on " << filePath << std::flush;
//        PrintingHandler::printOut(out, PrintingHandler::STDOUT);
//        delete out;
//
//        // Write the header
//        for(std::vector<std::string>::iterator it = solution->getHeader().begin();
//                it != solution->getHeader().end(); ++it){
//            std::string headerEntry = *it;
//            outFile << headerEntry << valuesSeparator;
//        }
//        outFile << entriesSeparator;
//
//        // Write the entries
//        for(std::vector<double*>::iterator it = solution->getSolutionList().begin();
//                it != solution->getSolutionList().end(); ++it){
//
//            double* entry = *it;
//
//            // Write one entry
//            for( int i = 0; i < solution->getN_VARS(); i++ ){
//                outFile << entry[i] << valuesSeparator;
//            }
//            outFile << entriesSeparator;
//        }
//
//        // Close the output file
//        outFile.close();
//
//    }
}
