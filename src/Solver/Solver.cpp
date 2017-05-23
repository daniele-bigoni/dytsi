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
 * File:   Solver.cpp
 * Author: s083781
 * 
 * Created on March 16, 2011, 4:33 PM
 */

#include "Solver.h"

#include "../GeneralModel/ExternalComponents.h"

#include "../Application/Input/XMLaux.h"
#include "../Application/PrintingHandler.h"

#include <sstream>

#include <boost/progress.hpp>

namespace DYTSI_Solver{
    Solver::Solver(DYTSI_Modelling::GeneralModel* gm, XERCES_CPP_NAMESPACE::DOMElement* el, int& error) {
        this->model = gm;
    }

    Solver::~Solver() {
    }

    int Solver::solve(int index, double* y0, DYTSI_Input::Range *tspan, Solution& sol){
        std::stringstream* out;
        if (!settedUp){
            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << "|============================================" << std::endl;
            *out << "|           Solver call information           " << std::endl;
            *out << "|============================================" << std::endl;
            *out << "| The solver has not been setted up!" << std::endl;
            *out << "|============================================" << std::endl << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            delete out;
            return 1;
        }

        this->model->init();
        int iterCounter = 0;
        double tOutput;
        double tOutputJac;
        double tProgressUpdate;
        double h = this->initStep;
        int n_var = this->model->get_N_DOF();
        int status = GSL_SUCCESS;

        double t = tspan->getStartValue();  // init time
        double tf = tspan->getEndValue(); // final time
        double totIntegTime = tf - t;

        // display info
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "|============================================" << std::endl;
        *out << "|           Solver call information           " << std::endl;
        *out << "|============================================" << std::endl;
        *out << "|  Time Span:          \t[" << t << "," << tf << "]" << std::endl;
        *out << "|  Initial Step Length:\t" << this->initStep << std::endl;
        *out << "|============================================" << std::endl;
        *out << "|           System Properties                " << std::endl;
        *out << "|============================================" << std::endl;
        *out << "|  Speed:              \t" << this->model->getExternalComponents()->get_v() << std::endl;
        *out << "|  Radius:              \t" << this->model->getExternalComponents()->get_R() << std::endl;
        *out << "|  Phi_se:             \t" << this->model->getExternalComponents()->get_Phi_se() << std::endl;
        *out << "|============================================" << std::endl << std::endl;
        PrintingHandler::printOut(out, PrintingHandler::STDOUT);
        delete out;

        // Store the elapsed time for this solution
        std::ostringstream* elapsedTime = new std::ostringstream();
        boost::progress_timer progressTimer(*elapsedTime);

        // Show the progress display
        boost::progress_display show_progress( 100 );

        // Copy initial conditions
        double *y = (double*)malloc(n_var*sizeof(double));
        memcpy(y,y0,n_var*sizeof(double));

        // Store initial conditions
        sol.addEntry(index, t, y, h);
        tOutput = t + this->storeFrequency;
        tOutputJac = t + this->jacStoreFrequency;
        tProgressUpdate = t + totIntegTime/100.0;
        status = this->init();
        if (status != GSL_SUCCESS){
            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << "Error in initializing the solver: " << gsl_strerror (status) << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            delete out;
        } else {
            while ((t < tf) && (status == GSL_SUCCESS)){
              iterCounter++;
              if (this->useDriver()){
                  // Step to the next output time
                  h = this->storeFrequency;
                  status = this->evolve(&t, tf, &h, y);
              } else {
                  // Perform one step
                  status = this->evolve(&t, tf, &h, y);
              }

              if (status != GSL_SUCCESS){
                  out = new std::stringstream(std::stringstream::in|std::stringstream::out);
                  *out << "Error: " << gsl_strerror (status) << std::endl;
                  *out << "Current iteration number: " << iterCounter << std::endl;
                  *out << "Model Counters: " << std::endl;
                  *out << this->model->printCounters();
                  *out << "Solver Counters: " << std::endl;
                  *out << this->showInfo();
                  PrintingHandler::printOut(out, PrintingHandler::STDOUT);
                  delete out;
              } else {
                  // Store the solution in simulation
                  if ((this->storeFrequency >= 0.0) && (t >= tOutput)){
                      sol.addEntry(index,t,y,h);
                      tOutput += this->storeFrequency;
                  }

                  // Store the jacobian
                  if ((this->jacStoreFrequency >= 0.0) && (t >= tOutputJac)){
                      int N_VARS = this->model->get_N_DOF();
                      double* dfdy = new double[N_VARS*N_VARS];
                      double* dfdt = new double[N_VARS];

                      this->model->computeJac(t,y,dfdy,dfdt);
                      sol.addJacobian(dfdy);
                      tOutputJac += this->jacStoreFrequency;

                      delete []dfdy;
                      delete []dfdt;
                  }

                  while (t > tProgressUpdate){
                      ++show_progress;
                      tProgressUpdate += totIntegTime / 100.0;
                  }
              }
            } // End While
            // Store last entry
            sol.addEntry(index, t, y, h);
            // End the progress update
            if (status == GSL_SUCCESS)
                ++show_progress;

            // Show the elapsed time
            progressTimer.~progress_timer();
            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << "Elapsed time for the solution: " << elapsedTime->str() << std::endl;
            *out << "Model Counters: " << std::endl;
            *out << this->model->printCounters();
            *out << "Solver Counters: " << std::endl;
            *out << this->showInfo();
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            delete out;
        }

        return status;
    }

    int Solver::solveTransient(int index, double* y0, DYTSI_Input::Range *tspan, Solution& sol, DYTSI_Input::TransientCurve* transientCurve){
        std::stringstream* out;
        if (!settedUp){
            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << "|============================================" << std::endl;
            *out << "|           Solver call information           " << std::endl;
            *out << "|============================================" << std::endl;
            *out << "| The solver has not been setted up!" << std::endl;
            *out << "|============================================" << std::endl << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            delete out;
            return 1;
        }

        this->model->init();
        int iterCounter = 0;
        double tOutput;
        double tOutputJac;
        double tProgressUpdate;
        double h = this->initStep;
        int n_var = this->model->get_N_DOF();
        int status = GSL_SUCCESS;

        double t = tspan->getStartValue();  // init time
        double tf = tspan->getEndValue(); // final time
        double totIntegTime = tf - t;

        // display info
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "|============================================" << std::endl;
        *out << "|           Solver call information           " << std::endl;
        *out << "|============================================" << std::endl;
        *out << "|  Time Span:          \t[" << t << "," << tf << "]" << std::endl;
        *out << "|  Initial Step Length:\t" << this->initStep << std::endl;
        *out << "|============================================" << std::endl;
        *out << "|           System Properties                " << std::endl;
        *out << "|============================================" << std::endl;
        *out << "|  Speed:              \t" << this->model->getExternalComponents()->get_v() << std::endl;
        if (!transientCurve->isSettedRadius()){
            *out << "|  Radius:              \t" << this->model->getExternalComponents()->get_R() << std::endl;
        } else {
            *out << "|  Transient Radius:" << std::endl;
            *out << "|  \t Start: \t " << transientCurve->getRadiusStart() << std::endl;
            *out << "|  \t End: \t " << transientCurve->getRadiusEnd() << std::endl;
            *out << "|  \t Time: \t " << transientCurve->getRadiusTime() << std::endl;
        }
        if (!transientCurve->isSettedCant()){
            *out << "|  Phi_se:             \t" << this->model->getExternalComponents()->get_Phi_se() << std::endl;
        } else {
            *out << "|  Transient Cant:" << std::endl;
            *out << "|  \t Start: \t " << transientCurve->getCantStart() << std::endl;
            *out << "|  \t End: \t " << transientCurve->getCantEnd() << std::endl;
            *out << "|  \t Time: \t " << transientCurve->getCantTime() << std::endl;
        }
        *out << "|============================================" << std::endl << std::endl;
        PrintingHandler::printOut(out, PrintingHandler::STDOUT);
        delete out;

        // Store the elapsed time for this solution
        std::ostringstream* elapsedTime = new std::ostringstream();
        boost::progress_timer progressTimer(*elapsedTime);

        // Show the progress display
        boost::progress_display show_progress( 100 );

        // Copy initial conditions
        double *y = (double*)malloc(n_var*sizeof(double));
        //for(int i = 0; i < n_var; i++) y[i] = y0[i];
        memcpy(y,y0,n_var*sizeof(double));

        // Store initial conditions
        sol.addEntry(index, t, y, h);
        tOutput = t + this->storeFrequency;
        tOutputJac = t + this->jacStoreFrequency;
        tProgressUpdate = t + totIntegTime/100.0;
        status = this->init();
        if (status != GSL_SUCCESS){
            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << "Error in initializing the solver: " << gsl_strerror (status) << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            delete out;
        } else {
            while ((t < tf) && (status == GSL_SUCCESS)){

                // Set up the transient curve
                if (t > 0){
                    if (transientCurve->isSettedRadius()){
                        double R = transientCurve->getRadiusEnd();
                        double dt = transientCurve->getRadiusTime();
                        double radius = (dt * R) / t;
                        this->model->getExternalComponents()->set_R(radius);
                    }
                    if (transientCurve->isSettedCant()){
                        double Phi_se = transientCurve->getCantEnd();
                        double dt = transientCurve->getCantTime();
                        double cant = (t * Phi_se) / dt;
                        this->model->getExternalComponents()->set_Phi_se(cant);
                    }
                }

                iterCounter++;
                status = this->evolve(&t, tf, &h, y);

                if (status != GSL_SUCCESS){
                  out = new std::stringstream(std::stringstream::in|std::stringstream::out);
                  *out << "error: " << gsl_strerror (status) << std::endl;
                  *out << "Current iteration number: " << iterCounter << std::endl;
                  *out << "Model Counters: " << std::endl;
                  *out << this->model->printCounters();
                  *out << "Solver Counters: " << std::endl;
                  *out << this->showInfo();
                  PrintingHandler::printOut(out, PrintingHandler::STDOUT);
                  delete out;
                } else {
                    // Store the solution in simulation
                    if ((this->storeFrequency >= 0.0) && (t >= tOutput)){
                      sol.addEntry(index, t,y,h);
                      tOutput += this->storeFrequency;
                    }

                    // Store the jacobian
                    if ((this->jacStoreFrequency >= 0.0) && (t >= tOutputJac)){
                      int N_VARS = this->model->get_N_DOF();
                      double* dfdy = new double[N_VARS*N_VARS];
                      double* dfdt = new double[N_VARS];

                      this->model->computeJac(t,y,dfdy,dfdt);
                      sol.addJacobian(dfdy);
                      tOutputJac += this->jacStoreFrequency;

                      delete []dfdy;
                      delete []dfdt;
                    }

                    while (t > tProgressUpdate){
                      ++show_progress;
                      tProgressUpdate += totIntegTime / 100.0;
                    }
                }
            } // End While
            // Store last entry
            if (this->storeFrequency < 0.0)
                sol.addEntry(index, t, y, h);
            // End the progress update
            if (status == GSL_SUCCESS)
                ++show_progress;

            // Show the elapsed time
            progressTimer.~progress_timer();
            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << "Elapsed time for the solution: " << elapsedTime->str() << std::endl;
            *out << "Model Counters: " << std::endl;
            *out << this->model->printCounters();
            *out << "Solver Counters: " << std::endl;
            *out << this->showInfo();
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            delete out;
        }

        return status;
    }

    int Solver::solveRamping(int index, double* y0, DYTSI_Input::Range *speedRange, Solution& sol){
    	std::stringstream* out;
		if (!settedUp){
			out = new std::stringstream(std::stringstream::in|std::stringstream::out);
			*out << "|============================================" << std::endl;
			*out << "|           Solver call information           " << std::endl;
			*out << "|============================================" << std::endl;
			*out << "| The solver has not been setted up!" << std::endl;
			*out << "|============================================" << std::endl << std::endl;
			PrintingHandler::printOut(out, PrintingHandler::STDOUT);
			delete out;
			return 1;
		}

		this->model->init();
		int iterCounter = 0;
		double tOutput;
		double tOutputJac;
		double tProgressUpdate;
		double h = this->initStep;
		int n_var = this->model->get_N_DOF();
		int status = GSL_SUCCESS;

		// The final time is computed from the linear decrease of the speed between the starting speed
		// and the ending speed.
		double t = 0.0; // init time
		double tf = (speedRange->getEndValue() - speedRange->getStartValue()) / speedRange->getCoeff();
		double totIntegTime = tf - t;

		// display info
		out = new std::stringstream(std::stringstream::in|std::stringstream::out);
		*out << "|============================================" << std::endl;
		*out << "|           Solver call information           " << std::endl;
		*out << "|============================================" << std::endl;
		*out << "|  Time Span:          \t[" << t << "," << tf << "]" << std::endl;
		*out << "|  Initial Step Length:\t" << this->initStep << std::endl;
		*out << "|============================================" << std::endl;
		*out << "|           System Properties                " << std::endl;
		*out << "|============================================" << std::endl;
		*out << "|  Speed:              \t" << this->model->getExternalComponents()->get_v() << std::endl;
		*out << "|  \t Decel Coeff: \t " << speedRange->getCoeff() << std::endl;
		*out << "|  \t Final speed: \t " << speedRange->getEndValue() << std::endl;
		*out << "|============================================" << std::endl << std::endl;
		PrintingHandler::printOut(out, PrintingHandler::STDOUT);
		delete out;

		// Store the elapsed time for this solution
		std::ostringstream* elapsedTime = new std::ostringstream();
		boost::progress_timer progressTimer(*elapsedTime);

		// Show the progress display
		boost::progress_display show_progress( 100 );

		// Copy initial conditions
		double *y = (double*)malloc(n_var*sizeof(double));
		memcpy(y,y0,n_var*sizeof(double));

		// Store initial conditions
		sol.addEntry(index, t, y, h);
		tOutput = t + this->storeFrequency;
		tOutputJac = t + this->jacStoreFrequency;
		tProgressUpdate = t + totIntegTime/100.0;
		status = this->init();
		if (status != GSL_SUCCESS){
			out = new std::stringstream(std::stringstream::in|std::stringstream::out);
			*out << "Error in initializing the solver: " << gsl_strerror (status) << std::endl;
			PrintingHandler::printOut(out, PrintingHandler::STDOUT);
			delete out;
		} else {
			while ((t < tf) && (status == GSL_SUCCESS)){

				// Set up the speed
				if (t > 0){
					double startSpeed = speedRange->getStartValue();
					double endSpeed = speedRange->getEndValue();
					double accelCoeff = speedRange->getCoeff();
					double speed = startSpeed + t*accelCoeff;
					this->model->getExternalComponents()->set_v(speed);
				}

				iterCounter++;
				status = this->evolve(&t, tf, &h, y);

				if (status != GSL_SUCCESS){
				  out = new std::stringstream(std::stringstream::in|std::stringstream::out);
				  *out << "error: " << gsl_strerror (status) << std::endl;
				  *out << "Current iteration number: " << iterCounter << std::endl;
				  *out << "Model Counters: " << std::endl;
				  *out << this->model->printCounters();
				  *out << "Solver Counters: " << std::endl;
				  *out << this->showInfo();
				  PrintingHandler::printOut(out, PrintingHandler::STDOUT);
				  delete out;
				} else {
					// Store the solution in simulation
					if ((this->storeFrequency >= 0.0) && (t >= tOutput)){
					  sol.addEntry(index, t,y,h);
					  tOutput += this->storeFrequency;
					}

					// Store the jacobian
					if ((this->jacStoreFrequency >= 0.0) && (t >= tOutputJac)){
					  int N_VARS = this->model->get_N_DOF();
					  double* dfdy = new double[N_VARS*N_VARS];
					  double* dfdt = new double[N_VARS];

					  this->model->computeJac(t,y,dfdy,dfdt);
					  sol.addJacobian(dfdy);
					  tOutputJac += this->jacStoreFrequency;

					  delete []dfdy;
					  delete []dfdt;
					}

					while (t > tProgressUpdate){
					  ++show_progress;
					  tProgressUpdate += totIntegTime / 100.0;
					}
				}
			} // End While
			// Store last entry
			if (this->storeFrequency < 0.0)
				sol.addEntry(index, t, y, h);
			// End the progress update
			if (status == GSL_SUCCESS)
				++show_progress;

			// Show the elapsed time
			progressTimer.~progress_timer();
			out = new std::stringstream(std::stringstream::in|std::stringstream::out);
			*out << "Elapsed time for the solution: " << elapsedTime->str() << std::endl;
			*out << "Model Counters: " << std::endl;
			*out << this->model->printCounters();
			*out << "Solver Counters: " << std::endl;
			*out << this->showInfo();
			PrintingHandler::printOut(out, PrintingHandler::STDOUT);
			delete out;
		}

		return status;
    }

    std::string Solver::getSolverInformation(){
        // display info
        std::stringstream* out;
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "|========================================" << std::endl;
        *out << "|           Solver Information           " << std::endl;
        *out << "|========================================" << std::endl;
        *out << "|  ODE solver: " << this->getSolverName() << std::endl;
        *out << "|  Abs. precision: " << this->absPrecision << std::endl;
        *out << "|  Rel. precision: " << this->relPrecision << std::endl;
        *out << "|  Store data every " << this->storeFrequency << " seconds" << std::endl;
        *out << "|========================================" << std::endl << std::endl;
        return out->str();
    }
}

