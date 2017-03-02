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
 * File:   BulirschStoerSolver.cpp
 * Author: s083781
 * 
 * Created on March 16, 2011, 5:35 PM
 */

#include "BulirschStoerSolver.h"

#include "../Application/Input/XMLaux.h"

#include <sstream>

namespace DYTSI_Solver {
    BulirschStoerSolver::BulirschStoerSolver(DYTSI_Modelling::GeneralModel* gm, XERCES_CPP_NAMESPACE::DOMElement* el, int& error)
    : Solver(gm, el, error){

        error += XMLaux::loadDoubleProperty(el, "InitStepLength", this->initStep);
        error += XMLaux::loadDoubleProperty(el, "StoreFrequency", this->storeFrequency);
        error += XMLaux::loadDoubleProperty(el, "AbsPrecision", this->absPrecision);
        error += XMLaux::loadDoubleProperty(el, "RelPrecision", this->relPrecision);
        error += XMLaux::loadDoubleProperty(el, "JacStoreFrequency", this->jacStoreFrequency);
        
    }

    BulirschStoerSolver::~BulirschStoerSolver() {
    }

    std::string BulirschStoerSolver::getSolverName() const{
        return std::string("Implicit Bulirsch-Stoer method of Bader and Deuflhard.");
    }
    
    const gsl_odeiv2_step_type* BulirschStoerSolver::getStepType(){
        return gsl_odeiv2_step_bsimp;
    }

    int BulirschStoerSolver::setup(){
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

    int BulirschStoerSolver::evolve(double* t, double tf, double* h, double* y){
        gsl_odeiv2_system sys = this->model->get_sys();
        return gsl_odeiv2_evolve_apply (e, c, s, &sys, t, tf, h, y);
    }

    int BulirschStoerSolver::init(){
        gsl_odeiv2_evolve_reset(e);
        return GSL_SUCCESS;
    }

    std::string BulirschStoerSolver::showInfo(){
        std::stringstream out(std::stringstream::in|std::stringstream::out);
        out << "Total Steps: " << e->count << std::endl;
        out << "Bad Steps: " << e->failed_steps << std::endl;
        std::string outStr = out.str();
        return outStr;
    }

    bool BulirschStoerSolver::useDriver(){
        return false;
    }
}
