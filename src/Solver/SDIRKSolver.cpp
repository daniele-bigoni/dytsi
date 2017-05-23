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
 * File:   SDIRKSolver.cpp
 * Author: bigo
 * 
 * Created on May 8, 2011, 5:05 PM
 */

#include "SDIRKSolver.h"

#include "gsl_odeiv_ext.h"

#include "../Application/Input/XMLaux.h"

#include <sstream>

namespace DYTSI_Solver {

    DYTSI_Modelling::GeneralModel* locModel;

    int funBinder(double t, DVector &y, DVector &f);
    int jacBinder(double t, DVector &y, DMatrix &dfdy);

    SDIRKSolver::SDIRKSolver(DYTSI_Modelling::GeneralModel* gm, XERCES_CPP_NAMESPACE::DOMElement* el, int& error, SDIRKMethodType type)
    : Solver(gm, el, error){
        locModel = gm;
        this->methodType = type;

        error += XMLaux::loadDoubleProperty(el, "InitStepLength", this->initStep);
        error += XMLaux::loadDoubleProperty(el, "StoreFrequency", this->storeFrequency);
        error += XMLaux::loadDoubleProperty(el, "AbsPrecision", this->absPrecision);
        error += XMLaux::loadDoubleProperty(el, "JacStoreFrequency", this->jacStoreFrequency);

    }

    SDIRKSolver::~SDIRKSolver() {
    }

    std::string SDIRKSolver::getSolverName() const{
        return std::string("SDIRK.");
    }

    const gsl_odeiv2_step_type* SDIRKSolver::getStepType(){
        return gsl_odeiv2_step_sdirk;
    }

    int SDIRKSolver::setup(){
        int n_var = this->model->get_N_DOF();

        this->SDIRKInstance = new Sdirk(
                this->absPrecision, n_var, &funBinder, &jacBinder, SC_PI, this->methodType);

        this->settedUp = true;

        return 0;
    }

    int SDIRKSolver::evolve(double* t, double tf, double* h, double* yVec){
        int errCode = GSL_SUCCESS;
        locModel = this->model;
        int n_var = this->model->get_N_DOF();

        // Copy standard vector to DVector
        DVector y(n_var);
        for(int i = 0; i < n_var; i++)
            y[i+1] = yVec[i];

        double finalTime = tf;

        if (this->isFirstStep){
            errCode = this->SDIRKInstance->Integrate(*t, *h, y);
            if (errCode != GSL_SUCCESS)
                return errCode;
            this->isFirstStep = false;
        } else {
            errCode = this->SDIRKInstance->Integrate(finalTime, *h, y);
            if (errCode != GSL_SUCCESS)
                return errCode;
            finalTime = tf;
        }
        *t += *h;

        // Copy back the values of y
        for(int i = 0; i < n_var; i++)
            yVec[i] = y[i+1];

        return errCode;
    }

    int SDIRKSolver::init(){
        this->SDIRKInstance->Reset();
        this->isFirstStep = true;
        return GSL_SUCCESS;
    }

    std::string SDIRKSolver::showInfo(){
        SdirkInfoType info;
        this->SDIRKInstance->GetInfo(info);

        std::stringstream out(std::stringstream::in|std::stringstream::out);
        out << "Good Steps: " << info.NumOfGoodStep << std::endl;
        out << "Bad Steps: " << info.NumOfBadStep << std::endl;
        out << "Newton divergence: " << info.NumOfNewtonDivergens << std::endl;
        out << "Max Error: " << info.MaxError << std::endl;
        std::string outStr = out.str();
        return outStr;
    }

    bool SDIRKSolver::useDriver(){
        return false;
    }

    int funBinder(double t, DVector &y, DVector &f){
        int status = locModel->computeFun(t, y.getVector(), f.getVector());

        return status;
    }

    int jacBinder(double t, DVector &y, DMatrix &dfdy){
        int n_var = locModel->get_N_DOF();

        double* dfdyVec = new double[n_var*n_var];
        double* dfdtVec = new double[n_var];

        int status = locModel->computeJac(t, y.getVector(), dfdyVec, dfdtVec);

        for(int i = 0; i < n_var; i++)
            for(int j = 0; j < n_var; j++)
                dfdy(i+1,j+1) = dfdyVec[(i*n_var)+j];

        delete[] dfdyVec;
        delete[] dfdtVec;

        return status;
    }

}
