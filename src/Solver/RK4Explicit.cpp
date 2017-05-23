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
 * File:   RK4Explicit.cpp
 * Author: bigo
 * 
 * Created on August 19, 2011, 11:34 AM
 */

#include "RK4Explicit.h"

#include "../Application/Input/XMLaux.h"

#include <sstream>

namespace DYTSI_Solver{
    RK4Explicit::RK4Explicit(DYTSI_Modelling::GeneralModel* gm,
                XERCES_CPP_NAMESPACE::DOMElement* el, int& error)
    : Solver(gm, el, error){

        error += XMLaux::loadDoubleProperty(el, "InitStepLength", this->initStep);
        error += XMLaux::loadDoubleProperty(el, "StoreFrequency", this->storeFrequency);
        error += XMLaux::loadDoubleProperty(el, "AbsPrecision", this->absPrecision);
        error += XMLaux::loadDoubleProperty(el, "RelPrecision", this->relPrecision);
        error += XMLaux::loadDoubleProperty(el, "JacStoreFrequency", this->jacStoreFrequency);
        
    }

    RK4Explicit::~RK4Explicit() {
    }

    std::string RK4Explicit::getSolverName() const{
        return std::string("Explicit 4th order Runge-Kutta");
    }

    const gsl_odeiv2_step_type* RK4Explicit::getStepType(){
        return gsl_odeiv2_step_rk4;
    }

    int RK4Explicit::setup(){
        int n_var = this->model->get_N_DOF();

        // Allocation of the step function
        this->s = gsl_odeiv2_step_alloc (this->getStepType(), n_var);
        // Definition of the control
        this->c = gsl_odeiv2_control_y_new (this->absPrecision, this->relPrecision);
        // Definition of the evolution function
        this->e = gsl_odeiv2_evolve_alloc(n_var);

        this->settedUp = true;

        return 0;
    }

    int RK4Explicit::evolve(double* t, double tf, double* h, double* y){
        gsl_odeiv2_system sys = this->model->get_sys();
        return gsl_odeiv2_evolve_apply (e, c, s, &sys, t, tf, h, y);
    }

    int RK4Explicit::init(){
        return GSL_SUCCESS;
    }

    std::string RK4Explicit::showInfo(){
        std::stringstream out(std::stringstream::in|std::stringstream::out);
        out << "Total Steps: " << e->count << std::endl;
        out << "Bad Steps: " << e->failed_steps << std::endl;
        std::string outStr = out.str();
        return outStr;
    }

    bool RK4Explicit::useDriver(){
        return false;
    }
}
