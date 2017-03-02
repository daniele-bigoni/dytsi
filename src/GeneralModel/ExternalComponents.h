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
 * File:   ExternalComponents.h
 * Author: bigo
 *
 * Created on February 12, 2011, 3:54 PM
 */

#ifndef EXTERNALCOMPONENTS_H
#define	EXTERNALCOMPONENTS_H

#include <string>
#include <vector>

namespace DYTSI_Modelling {
    class ExternalComponents {
    public:
        ExternalComponents( double v, double R, double Phi_se);
        virtual ~ExternalComponents();

        static const int N_VARS = 3;
        static const int posV = 0;
        static const int posR = 1;
        static const int posPhi_se = 2;
        static const std::string* varsName[N_VARS];

        double get_v(){ return v; }
        void set_v( double v ){ this->v = v; }
        double get_R(){ return R; }
        void set_R( double R ){ this->R = R; }
        double get_Phi_se(){ return Phi_se; }
        void set_Phi_se( double Phi_se );
        double get_sin_Phi_se(){ return sin_Phi_se; }
        double get_cos_Phi_se(){ return cos_Phi_se; }

        std::vector<std::string> getHeader();
        void getStatus(double* status);

    private:
        double v;
        double R;
        double Phi_se;
        double sin_Phi_se;
        double cos_Phi_se;

    };
}

#endif	/* EXTERNALCOMPONENTS_H */

