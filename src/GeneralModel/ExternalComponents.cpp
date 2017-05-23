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
 * File:   ExternalComponents.cpp
 * Author: bigo
 * 
 * Created on February 12, 2011, 3:54 PM
 */

#include "ExternalComponents.h"

#include <sstream>

#include <cmath>

namespace DYTSI_Modelling {
    const std::string* ExternalComponents::varsName[N_VARS] = { new std::string("V"), new std::string("R"), new std::string("Phi_se")};

    ExternalComponents::ExternalComponents( double v, double R, double Phi_se) {
        this->v = v;
        this->R = R;
        this->Phi_se = Phi_se;
        this->sin_Phi_se = sin(Phi_se);
        this->cos_Phi_se = cos(Phi_se);
    }

    ExternalComponents::~ExternalComponents() {
    }

    std::vector<std::string> ExternalComponents::getHeader(){
        std::vector<std::string> out;
        for( int i = 0; i < this->N_VARS; i++){
            std::stringstream* stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
            const std::string* str = this->varsName[i];
            *stream << "ExternalComponents::" << *str;
            out.push_back(stream->str());
            delete stream;
        }
        return out;
    }

    void ExternalComponents::getStatus(double* status){
        status[this->posV] = this->v;
        status[this->posR] = this->R;
        status[this->posPhi_se] = this->Phi_se;
    }

    void ExternalComponents::set_Phi_se( double Phi_se ){
        this->Phi_se = Phi_se;
        this->sin_Phi_se = sin(Phi_se);
        this->cos_Phi_se = cos(Phi_se);
    }
}
