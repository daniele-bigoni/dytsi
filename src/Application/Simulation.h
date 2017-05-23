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

#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdlib.h>

#include "Input/InputHandler.h"

// Global variables
enum ExecMode {NO_THREADS, THREADS};

void printRsgeo();

class Simulation{

public:
    Simulation(int argc, char* argv[]);
    Simulation();
    ~Simulation();

    void readInput(int argc, char* argv[]);
    void startSimulations();
//    void transientAnalysis(DYTSI_Modelling::GeneralModel model,
//                InputStruct in, const char* outPath, int outType, int i);
//    void bifurcationAnalysis(DYTSI_Modelling::GeneralModel model,
//                InputStruct in, const char* outPath, int outType, int i);

    bool getInput_loaded();

    static ExecMode getExecMode(){ return execMode; }

private:
    DYTSI_Input::InputHandler* inputHandler;
    
    bool input_loaded;
    
    static int mode; // 0 Text mode, 1 Graphic mode
    static ExecMode execMode; // 0 Non-Thread, 1 Thread mode

//    void toMatlabFile(const char* file, int simN, InputStruct in, std::vector<SolEntry>& sim);
//    void toGnuPlotFile(const char* file, int simN, InputStruct in, std::vector<SolEntry>& sim, bool plot);
};

#endif
