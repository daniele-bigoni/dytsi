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

#include "Simulation.h"
#include "miscellaneous.h"

#include "Input/InputHandler.h"
#include "PrintingHandler.h"
#include "../GeneralModel/DYTSI_Modelling.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

#include "time.h"
#include "string.h"
#include "Output/OutputHandler.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>


#define PI 3.14159265

using namespace std;

//const char MATLAB_SEPARATOR = ',';
//const char MATLAB_NEWENTRY[] = "; ";
//const char MATLAB_COMMENT = '%';
//const char GNUPLOT_SEPARATOR = ' ';
//const char GNUPLOT_NEWENTRY[] = "\n";
//const char GNUPLOT_COMMENT = '#';

extern int RunningMode_graphics;

int Simulation::mode = 0;
ExecMode Simulation::execMode = NO_THREADS;

void Simulation::readInput(int argc, char* argv[]){
    bool error = false;

    // Elaborate inputs
    std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
    *out << "================================================" << std::endl;
    *out << "Starting the elaboration of the Input" << std::endl;
    PrintingHandler::printOut(out,PrintingHandler::STDOUT);
    delete out;

    this->inputHandler = new DYTSI_Input::InputHandler();
    inputHandler->reset();
    for (int i=0; i < argc; i++){
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << i << ") File: " << argv[i] << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        error = inputHandler->parseFile(argv[i]);
        if (error){
            this->input_loaded = false;
            return;
        }
    }
    this->input_loaded = true;

    out = new std::stringstream(std::stringstream::in|std::stringstream::out);
    *out << "Finished the elaboration of the Input" << std::endl;
    *out << "================================================" << std::endl << std::endl;
    PrintingHandler::printOut(out,PrintingHandler::STDOUT);
    delete out;
}

void Simulation::startSimulations(){

    // Start the simulations
    std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
    *out << "================================================" << std::endl;
    *out << "Starting the Simulations" << std::endl << std::endl;
    PrintingHandler::printOut(out,PrintingHandler::STDOUT);
    delete out;

    // Iterate over the inputs
    //for( std::vector<DYTSI_Input::Input>::iterator input = (this->inputHandler->getInputList()).begin();
    //        input != this->inputHandler->getInputList().end(); ++input){
    std::vector<DYTSI_Input::Input*> inputList = this->inputHandler->getInputList();
    for(unsigned int i = 0; i < inputList.size(); i++){

        DYTSI_Input::Input* input = inputList[i];

        
        input->simulate();

        //input->getOutputHandler()->writeSolution(sol);

    }

    out = new std::stringstream(std::stringstream::in|std::stringstream::out);
    *out << std::endl << "Finished the Simulations" << std::endl;
    *out << "================================================" << std::endl << std::endl;
    PrintingHandler::printOut(out,PrintingHandler::STDOUT);
    delete out;

}

Simulation::Simulation(int argc, char* argv[]){

  this->mode = 0;

  cout << "Starting the program" << endl;

  if (argc < 4){
    cout << "Not enough input arguments!" << endl;
    cout << "USAGE: ./simulation -<GraphicMode> -<ThreadMode> <INPUT_file_1>.xml ... <INPUT_file_n>.xml" << endl;
    cout << "USAGE: Possible Graphic modes: G, T" << endl;
    cout << "USAGE: Possible Thread modes: nt, pt" << endl;
    exit(1);
  }

  // Set the Thread mode
  if (strcmp(argv[2],"-nt") == 0){
        cout << "Thread mode: OFF" << endl;
        Simulation::execMode = NO_THREADS;
  } else if (strcmp(argv[2],"-pt") == 0){
        cout << "Thread mode: ON" << endl;
        Simulation::execMode = THREADS;
  } else {
        cout << "Wrong arguments!" << endl;
        cout << "USAGE: ./simulation -<GraphicMode> -<ThreadMode> <INPUT_file_1>.xml ... <INPUT_file_n>.xml" << endl;
        cout << "USAGE: Possible Graphic modes: G, T" << endl;
        cout << "USAGE: Possible Thread modes: nt, pt" << endl;
        exit(1);
  }

  this->readInput(argc-3, &argv[3]);
  if (!this->input_loaded){
      std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
      *out << "Error: the input has not been correctly loaded." << std::endl;
      *out << "Program Termination \t [DONE]" << std::endl;
      PrintingHandler::printOut(out,PrintingHandler::STDOUT);
      delete out;
      return;
  }

  this->startSimulations();
  
}

Simulation::Simulation(){
    this->mode = 1;
}

Simulation::~Simulation(){

}
