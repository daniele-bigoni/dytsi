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
 * File:   OutputHandler.h
 * Author: s083781
 *
 * Created on March 16, 2011, 10:35 AM
 */

#ifndef OUTPUTHANDLER_H
#define	OUTPUTHANDLER_H

#include <xercesc/dom/DOM.hpp>
#include <vector>
#include <string>

namespace DYTSI_Input {
    class Input;
}

namespace DYTSI_Output {
    class OutputHandler {
    public:
        OutputHandler( DYTSI_Input::Input* input, XERCES_CPP_NAMESPACE::DOMElement* node, int& error);
        virtual ~OutputHandler();

        void reset();
        void writeSolutionHeader(std::vector<std::string> header);
        void writeSolutionEntry(double* entry, int N_VARS);
        void writeJacobian(double* dfdy, int N_VARS, int idx);
        //void writeSolution(DYTSI_Solver::Solution* solution);

        static const int N_FORMAT_TYPE = 1;
        static const int N_PLOTTING_TYPE = 2;
        static const std::string* FormatTypeNames[N_FORMAT_TYPE];
        static const std::string* PlottingTypeNames[N_PLOTTING_TYPE];

        enum FormatType {TAB};
        enum PlottingType {NONE, GNUPlot};

    private:
        DYTSI_Input::Input* input;
        std::string solPathFile;
        std::string jacPathDir;

        char* valuesSeparator;
        char* entriesSeparator;

        FormatType format;
        PlottingType plotting;
    };
}

#endif	/* OUTPUTHANDLER_H */

