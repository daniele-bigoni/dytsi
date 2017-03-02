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
 * File:   PieceWiseValues.cpp
 * Author: s083781
 * 
 * Created on May 2, 2011, 2:55 PM
 */

#include "PieceWiseValues.h"

#include "../Application/PrintingHandler.h"

#include <sstream>

namespace DYTSI_Modelling {
    PieceWiseValues::PieceWiseValues(double** vals, int size, std::string interpTypeStr) {
        this->vals = vals;
        this->size = size;

        if (interpTypeStr.compare("linear") == 0){
            this->interpType = DYTSI_Modelling::INTP_LINEAR;
        } else if (interpTypeStr.compare("cspline") == 0){
            this->interpType = DYTSI_Modelling::INTP_CUBICSPLINE;
        } else if (interpTypeStr.compare("akima") == 0){
            this->interpType = DYTSI_Modelling::INTP_AKIMA;
        } else {
            std::stringstream out;
            out << "Linear Interpolation assumed for Piece Wise Value Function." << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDOUT);
            this->interpType = DYTSI_Modelling::INTP_LINEAR;
        }
    }

    PieceWiseValues::~PieceWiseValues() {
    }

    DYTSI_Modelling::InterpolationTypes PieceWiseValues::getInterpType(){
        return this->interpType;
    }
}
