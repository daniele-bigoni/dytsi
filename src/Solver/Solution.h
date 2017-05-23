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

/* 
 * File:   Solution.h
 * Author: s083781
 *
 * Created on March 16, 2011, 2:14 PM
 */

#ifndef SOLUTION_H
#define	SOLUTION_H

#include "../GeneralModel/GeneralModel.h"

#include <vector>
#include <string>

namespace DYTSI_Output{
    class OutputHandler;
}

namespace DYTSI_Solver{
    class Solution {
    public:
        Solution(DYTSI_Modelling::GeneralModel* gm, DYTSI_Output::OutputHandler* outputHandler);
        virtual ~Solution();

        void reset();

        std::vector<std::string> getHeader();
        std::vector<double*> getSolutionList();
        std::vector<double*> getStatusList();
        int getN_VARS();
        void addEntry(int index, double t, double* y, double h);
        void addJacobian(double* dfdy);
        
    private:
        int N_VARS;

        int indexPosition;
        int tPosition; // Position in the entry occupied by t
        int tStepPosition; // Position of the time step variable
        int extComponentStartingPosition;
        
        DYTSI_Modelling::GeneralModel* gm;
        DYTSI_Output::OutputHandler* outputHandler;

        std::vector<std::string> header;
        std::vector<double*> solList;
        std::vector<double*> statusList;

        std::vector<double*> jacList;

        std::vector<DYTSI_Modelling::Component*> componentList;
        std::vector<int> componentsStartingPosition;

        void setComponentList();
        void setHeader();
    };
}

#endif	/* SOLUTION_H */

