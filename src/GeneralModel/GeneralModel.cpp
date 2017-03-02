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
 * File:   GeneralModel.cpp
 * Author: Daniele Bigoni
 * 
 * Created on February 12, 2011, 5:03 PM
 */

#include "GeneralModel.h"
#include "DYTSI_Modelling.h"

#include "../Application/PrintingHandler.h"
#include "../Application/Simulation.h"
#include "../Application/Input/XMLaux.h"

#include <sstream>

namespace DYTSI_Modelling{
    int fun(double t, const double y[], double f[], void *params);
    int jac(double t, const double y[], double *dfdy, double dfdt[], void *params);

//  int fun_acceleration(double t, const double y[], double f[], void *params);
//	int jac_acceleration(double t, const double y[], double *dfdy, double dfdt[], void *params);

    GeneralModel::GeneralModel() {
        N_DOF = 0;
        this->externalComponents = new ExternalComponents(0.1, 1e99, 0.0);
        this->settedUp = false;
    }

    GeneralModel::~GeneralModel() {
    }

    void GeneralModel::setup(XERCES_CPP_NAMESPACE::DOMElement* el){
        this->staticLoadedPrimarySuspensionSpringList.clear();
        this->staticLoadedSecondarySuspensionSpringList.clear();

        bool error;
        error = this->setupModel(el);
        if (error) return;
        error = this->setupStaticLoads();
        if (error) return;
        gsl_odeiv2_system system = {fun, jac, N_DOF, this};
        this->sys = system;
        this->setSettedUp(true);
    }

    bool GeneralModel::setStartingValues(XERCES_CPP_NAMESPACE::DOMElement* el, std::vector<double>& startingValues){
        return this->loadStartingValues(el, startingValues);
    }

//    /*
//     * Update the system such that the acceleration is taken into account inside the system of equations.
//     * This means that one DOF need to be added, accounting for the speed v.
//     */
//    bool GeneralModel::setAcceleration(){
//    	this->N_DOF++;
//
//    	gsl_odeiv2_system system = {fun_acceleration, jac_acceleration, N_DOF, this};
//    	this->sys = system;
//		this->setSettedUp(true);
//
//		return true;
//    	// TODO Fix computeFun, such that the last entry will be set to the decel coeff and all the other entries will be computed according to the speed received in input!!
//    }

    void GeneralModel::init(){
        this->fun_counter = 0;
        this->jac_counter = 0;
        this->getRootComponent()->init();
    }

    std::string GeneralModel::printCounters(){
        std::stringstream out(std::stringstream::in|std::stringstream::out);
        out << "Function evaluations: \t" << this->fun_counter << std::endl;
        out << "Jacobian evaluations: \t" << this->jac_counter << std::endl;
        out << this->getRootComponent()->printCounters();
        std::string outStr = out.str();
        return outStr;
    }

    void GeneralModel::setSettedUp(bool val){
        this->settedUp = val;
    }

    bool GeneralModel::getSettedUp() const{
        return this->settedUp;
    }

    int GeneralModel::computeFun(double t, const double y[], double f[]){
        this->fun_counter++;
        if (this->settedUp){
            // Check the exec mode
            double* yCopy = new double[this->get_N_DOF()];
            memcpy(yCopy, y, this->get_N_DOF() * sizeof(double));
            if (Simulation::getExecMode() == NO_THREADS){
                int errCode = ( this->getRootComponent()->fun(t, yCopy, f) );

                delete[] yCopy;

                return errCode;
            } else {
                // Pack the input
                FunInput funInput = {t, yCopy, f};
                ExecInput execInput;
                execInput.type = FUN;
                execInput.input = &funInput;

                // Start the root thread
                this->getRootComponent()->Start( &execInput );

                // Wait for finishing
                this->getRootComponent()->Join();

                delete[] yCopy;

                return execInput.returnCode;
            }
        } else {
            std::stringstream out;
            out << "Error: GeneralModel::computeFun. The Model is not setted up." << std::endl;
            out << "Hint: check to have called the function GeneralModel::setup()" << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDERR);
            return GSL_EINVAL;
        }
    }

    int GeneralModel::computeJac(double t, const double y[], double* dfdy, double dfdt[]){
        this->jac_counter++;
        if (this->settedUp){
            // Check the exec mode
            double* yCopy = new double[this->get_N_DOF()];
            memcpy(yCopy, y, this->get_N_DOF() * sizeof(double));
            if (Simulation::getExecMode() == NO_THREADS){
                int errCode = ( this->getRootComponent()->jac(t, yCopy, dfdy, dfdt) );

                delete[] yCopy;

                return errCode;
            } else {
                // Pack the input
                JacInput jacInput = {t, yCopy, dfdy, dfdt};
                struct ExecInput execInput;
                execInput.type = JAC;
                execInput.input = &jacInput;

                // Start the root thread
                this->getRootComponent()->Start( &execInput );

                // Wait for finishing
                this->getRootComponent()->Join();

                delete[] yCopy;

                return execInput.returnCode;
            }
        } else {
            std::stringstream out;
            out << "Error: GeneralModel::computeFun. The Model is not setted up." << std::endl;
            out << "Hint: check to have called the function GeneralModel::setup()" << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDERR);
            return GSL_EINVAL;
        }

    }

    int GeneralModel::computeFun(double t, double y[], double f[]){
        this->fun_counter++;
        if (this->settedUp){
            // Check the exec mode
            double* yCopy = new double[this->get_N_DOF()];
            memcpy(yCopy, y, this->get_N_DOF() * sizeof(double));
            if (Simulation::getExecMode() == NO_THREADS){
                int errCode = ( this->getRootComponent()->fun(t, yCopy, f) );

                delete[] yCopy;

                return errCode;
            } else {
                // Pack the input
                FunInput funInput = {t, yCopy, f};
                ExecInput execInput;
                execInput.type = FUN;
                execInput.input = &funInput;

                // Start the root thread
                this->getRootComponent()->Start( &execInput );

                // Wait for finishing
                this->getRootComponent()->Join();

                delete[] yCopy;

                return execInput.returnCode;
            }
        } else {
            std::stringstream out;
            out << "Error: GeneralModel::computeFun. The Model is not setted up." << std::endl;
            out << "Hint: check to have called the function GeneralModel::setup()" << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDERR);
            return GSL_EINVAL;
        }
    }

    int GeneralModel::computeJac(double t, double y[], double* dfdy, double dfdt[]){
        this->jac_counter++;
        if (this->settedUp){
            // Check the exec mode
            double* yCopy = new double[this->get_N_DOF()];
            memcpy(yCopy, y, this->get_N_DOF() * sizeof(double));
            if (Simulation::getExecMode() == NO_THREADS){
                int errCode = ( this->getRootComponent()->jac(t, yCopy, dfdy, dfdt) );

                delete[] yCopy;

                return errCode;
            } else {
                // Pack the input
                JacInput jacInput = {t, yCopy, dfdy, dfdt};
                struct ExecInput execInput;
                execInput.type = JAC;
                execInput.input = &jacInput;

                // Start the root thread
                this->getRootComponent()->Start( &execInput );

                // Wait for finishing
                this->getRootComponent()->Join();

                delete[] yCopy;

                return execInput.returnCode;
            }
        } else {
            std::stringstream out;
            out << "Error: GeneralModel::computeFun. The Model is not setted up." << std::endl;
            out << "Hint: check to have called the function GeneralModel::setup()" << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDERR);
            return GSL_EINVAL;
        }

    }

    int GeneralModel::setRootComponent( Component* component ){
        this->rootComponent = component;
        return 1;
    }

    // Only the vertical component are considered (all the vertical springs need to have the same characteristics!)
    bool GeneralModel::setupStaticLoads(){
        bool error = false;
        double Fg;
        int nSprings;

        DYTSI_Modelling::CarBodyComponent* carBody = (DYTSI_Modelling::CarBodyComponent*) this->getRootComponent();
        DYTSI_Modelling::BogieFrameComponent* leadingBogieFrame = (DYTSI_Modelling::BogieFrameComponent*) carBody->getLowerLeading()->getComponent();
        DYTSI_Modelling::BogieFrameComponent* trailingBogieFrame = (DYTSI_Modelling::BogieFrameComponent*) carBody->getLowerTrailing()->getComponent();

        // Setup static load of the secondary suspension
        Fg = - (carBody->get_mass() * g);
        nSprings = this->staticLoadedSecondarySuspensionSpringList.size();
        for(int i = 0; i < nSprings; i++){
            DYTSI_Modelling::LinkSpring* spring = this->staticLoadedSecondarySuspensionSpringList[i];
            error = spring->setStaticLoad(Fg/nSprings);
            if (error) return error;
        }

        // Setup static load for the primary suspension
        Fg = - ((carBody->get_mass() * g) + (leadingBogieFrame->get_mass() * g) + (trailingBogieFrame->get_mass() * g));
        nSprings = this->staticLoadedPrimarySuspensionSpringList.size();
        for(int i = 0; i < nSprings; i++){
            DYTSI_Modelling::LinkSpring* spring = this->staticLoadedPrimarySuspensionSpringList[i];
            error = spring->setStaticLoad(Fg/nSprings);
            if (error) return error;
        }

        return error;
    }

    int fun(double t, const double y[], double f[], void *params){
        GeneralModel* m = (GeneralModel *)params;
        int errCode = m->computeFun(t, y, f);
        if (errCode != GSL_SUCCESS)
            return GSL_EBADFUNC;
        return errCode;
    }

    int jac(double t, const double y[], double *dfdy, double dfdt[], void *params){
        GeneralModel* m = (GeneralModel *)params;
        int errCode = m->computeJac(t, y, dfdy, dfdt);
        if (errCode != GSL_SUCCESS)
            return GSL_EBADFUNC;
        return errCode;
    }

//    int fun_acceleration(double t, const double y[], double f[], void *params){
//		GeneralModel* m = (GeneralModel *)params;
//		int errCode = m->computeFun(t, y, f);
//		if (errCode != GSL_SUCCESS)
//			return GSL_EBADFUNC;
//		return errCode;
//	}
//
//	int jac_acceleration(double t, const double y[], double *dfdy, double dfdt[], void *params){
//		GeneralModel* m = (GeneralModel *)params;
//		int errCode = m->computeJac(t, y, dfdy, dfdt);
//		if (errCode != GSL_SUCCESS)
//			return GSL_EBADFUNC;
//		return errCode;
//	}

}
