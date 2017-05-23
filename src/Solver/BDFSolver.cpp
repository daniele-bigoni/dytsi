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
 * File:   BDFSolver.cpp
 * Author: s083781
 * 
 * Created on August 2, 2011, 5:55 PM
 */

#include "BDFSolver.h"

#include "../Application/Input/XMLaux.h"

#include <sstream>

namespace DYTSI_Solver {

    const double BDFSolver::HMIN = 1e-10;
    const double BDFSolver::NMAX = 1000;

    BDFSolver::BDFSolver(DYTSI_Modelling::GeneralModel* gm, XERCES_CPP_NAMESPACE::DOMElement* el, int& error)
    : Solver(gm, el, error){
        error += XMLaux::loadDoubleProperty(el, "InitStepLength", this->initStep);
        error += XMLaux::loadDoubleProperty(el, "StoreFrequency", this->storeFrequency);
        error += XMLaux::loadDoubleProperty(el, "AbsPrecision", this->absPrecision);
        error += XMLaux::loadDoubleProperty(el, "RelPrecision", this->relPrecision);
        error += XMLaux::loadDoubleProperty(el, "JacStoreFrequency", this->jacStoreFrequency);
    }

    BDFSolver::~BDFSolver() {
    }

    std::string BDFSolver::getSolverName() const{
        return std::string("BDF method in Nordsieck form.");
    }

    const gsl_odeiv2_step_type* BDFSolver::getStepType(){
        return gsl_odeiv2_step_msbdf;
    }

    int BDFSolver::setup(){
        gsl_odeiv2_system sys = this->model->get_sys();

        // Allocation of the driver
        this->d = gsl_odeiv2_driver_alloc_y_new (&sys, this->getStepType(),
     				  1e-6, this->absPrecision, this->relPrecision);

        gsl_odeiv2_driver_set_hmin(this->d, this->HMIN );
        gsl_odeiv2_driver_set_nmax(this->d, this->NMAX );

        this->settedUp = true;

        return 0;
    }

    int BDFSolver::evolve(double* t, double tf, double* h, double* y){
        gsl_odeiv2_system sys = this->model->get_sys();
        this->d->sys = &sys;
        double ti = *t + *h;
        int status = gsl_odeiv2_driver_apply (this->d, t, ti, y);
        *h = this->d->h;

        return status;
    }

    int BDFSolver::init(){
        gsl_odeiv2_driver_reset(this->d);
        return GSL_SUCCESS;
    }

    std::string BDFSolver::showInfo(){
        std::stringstream out(std::stringstream::in|std::stringstream::out);
        out << "Total Steps: " << this->d->e->count << std::endl;
        out << "Bad Steps: " << this->d->e->failed_steps << std::endl;
        std::string outStr = out.str();
        return outStr;
    }

    bool BDFSolver::useDriver(){
        return true;
    }
}

