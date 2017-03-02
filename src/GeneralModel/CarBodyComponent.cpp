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
 * File:   CarBodyComponent.cpp
 * Author: bigo
 * 
 * Created on February 12, 2011, 4:56 PM
 */

#include "CarBodyComponent.h"
#include "BogieFrameCarBodyConnector.h"
#include "ExternalComponents.h"
#include "GeneralModel.h"
#include "BogieFrameComponent.h"
#include "ComponentConnectorCouple.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

#include "../Application/PrintingHandler.h"
#include "../Application/Simulation.h"
#include "../Application/Input/XMLaux.h"

#include <cmath>

namespace DYTSI_Modelling {
    const std::string* CarBodyComponent::DOF_Names[N_DOF] = {   //new std::string("X"), new std::string("XDOT"),
                                                                new std::string("Y"), new std::string("YDOT"),
                                                                new std::string("Z"), new std::string("ZDOT"),
                                                                new std::string("PHI"), new std::string("PHIDOT"),
                                                                new std::string("CHI"), new std::string("CHIDOT"),
                                                                new std::string("PSI"), new std::string("PSIDOT")};

    CarBodyComponent::CarBodyComponent( gsl_vector* centerOfGeometry,
            gsl_vector* centerOfMass,
            double mass,
            double Ix,
            double Iy,
            double Iz,
            ExternalComponents* externalComponents,
            std::string name,
            GeneralModel* gm,
            bool isFixed)
    : Component(name, gm, externalComponents, centerOfGeometry, centerOfMass, isFixed) {
        this->setDOF(gm);

        this->mass = mass;
        this->Ix = Ix;
        this->Iy = Iy;
        this->Iz = Iz;

        // Allocation of the Transformation Matrices
        this->locTransformationMatrix = gsl_matrix_calloc(3,3);
        for ( int i = 0; i < 3; i++ ) gsl_matrix_set(this->locTransformationMatrix,i,i,1.0);
        this->locDTransformationMatrix = gsl_matrix_calloc(3,3);
        this->llTransformationMatrix = gsl_matrix_calloc(3,3);
        for ( int i = 0; i < 3; i++ ) gsl_matrix_set(this->llTransformationMatrix,i,i,1.0);
        this->llDTransformationMatrix = gsl_matrix_calloc(3,3);
        this->ltTransformationMatrix = gsl_matrix_calloc(3,3);
        for ( int i = 0; i < 3; i++ ) gsl_matrix_set(this->ltTransformationMatrix,i,i,1.0);
        this->ltDTransformationMatrix = gsl_matrix_calloc(3,3);

        // Allocation of the displacement vectors
        this->locDisp = gsl_vector_calloc(3);
        this->locDDisp = gsl_vector_calloc(3);
        this->llDisp = gsl_vector_calloc(3);
        this->llDDisp = gsl_vector_calloc(3);
        this->ltDisp = gsl_vector_calloc(3);
        this->ltDDisp = gsl_vector_calloc(3);

        // aux vectors
        this->vecForceAndMoment = gsl_vector_alloc(6);

        ///////////////////////////////////////////////////////
        // Set up interdependencies for the numerical Jacobian

        // Local variables
        //this->partialJacobianIdx.push_back(this->get_startingDOF()+X);
        //this->partialJacobianIdx.push_back(this->get_startingDOF()+XDOT);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+Y);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+YDOT);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+Z);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+ZDOT);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+PHI);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+PHIDOT);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+CHI);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+CHIDOT);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+PSI);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+PSIDOT);
        /////////////////////////////

        this->recalc = true;
    }

    CarBodyComponent::~CarBodyComponent() {
    }

    std::vector<std::string> CarBodyComponent::getHeader(){
        std::vector<std::string> out;
        for( int i = 0; i < this->N_DOF; i++){
            std::stringstream* stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
            const std::string* str = this->DOF_Names[i];
            *stream << this->get_name() << "::" << *str;
            out.push_back(stream->str());
            delete stream;
        }

        // Add connector forces And Moments
        std::stringstream* stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CONN-FX";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CONN-FY";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CONN-FZ";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CONN-MX";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CONN-MY";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CONN-MZ";
        out.push_back(stream->str());
        delete stream;

        // Add Gravitational Forces and Moments
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "GRAV-FX";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "GRAV-FY";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "GRAV-FZ";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "GRAV-MX";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "GRAV-MY";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "GRAV-MZ";
        out.push_back(stream->str());
        delete stream;

        // Add Centrifugal Forces and Moments
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CENT-FX";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CENT-FY";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CENT-FZ";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CENT-MX";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CENT-MY";
        out.push_back(stream->str());
        delete stream;
        stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *stream << this->get_name() << "::" << "CENT-MZ";
        out.push_back(stream->str());
        delete stream;

        return out;
    }

    void CarBodyComponent::getStatus(double t, double* y, double* status){
        // Output the DOF part
        memcpy(status, &y[this->get_startingDOF()], this->N_DOF * sizeof(double));

        ////////////////////////////////
        // Update all the forces
        int errCode = GSL_SUCCESS;

        // Set up shortcuts
        this->llComponent = (DYTSI_Modelling::BogieFrameComponent*)
                this->getLowerLeading()->getComponent();
        this->llConnector = (DYTSI_Modelling::BogieFrameCarBodyConnector*)
                this->getLowerLeading()->getConnector();
        this->ltComponent = (DYTSI_Modelling::BogieFrameComponent*)
                this->getLowerTrailing()->getComponent();
        this->ltConnector = (DYTSI_Modelling::BogieFrameCarBodyConnector*)
                this->getLowerTrailing()->getConnector();

        // Set locY, llY, ltY and upY
        this->locY = &(y[this->get_startingDOF()]);
        this->llY = &(y[this->llComponent->get_startingDOF()]);
        this->ltY = &(y[this->ltComponent->get_startingDOF()]);

        // Update the transformation matrices
        this->updateTransformationMatrix(this->locTransformationMatrix, y);
        this->updateDTransformationMatrix(this->locDTransformationMatrix, y);
        this->llComponent->updateTransformationMatrix(this->llTransformationMatrix, y);
        this->llComponent->updateDTransformationMatrix(this->llDTransformationMatrix, y);
        this->ltComponent->updateTransformationMatrix(this->ltTransformationMatrix, y);
        this->ltComponent->updateDTransformationMatrix(this->ltDTransformationMatrix, y);

        // Update the displacement vectors
        this->updateDisplacementVector(this->locDisp, y);
        this->updateDDisplacementVector(this->locDDisp, y);
        this->llComponent->updateDisplacementVector(this->llDisp, y);
        this->llComponent->updateDDisplacementVector(this->llDDisp, y);
        this->ltComponent->updateDisplacementVector(this->ltDisp, y);
        this->ltComponent->updateDDisplacementVector(this->ltDDisp, y);

        // Compute the connectors forces
        errCode = this->computeConnectorsForcesAndMoments();

        // Compute Gravitational forces and moments
        gsl_vector* gravitationalForceAndMoment = this->computeGravitationalForceAndMoment();

        // Compute Centrifugal forces and moments
        gsl_vector* centrifugalForceAndMoment = this->computeCentrifugalForceAndMoment();
        ///////////////////////////////

        int pos = this->get_N_DOF();
        // Output the Connector forces
        status[pos++] = gsl_vector_get(this->connectorsForcesAndMoments,0);
        status[pos++] = gsl_vector_get(this->connectorsForcesAndMoments,1);
        status[pos++] = gsl_vector_get(this->connectorsForcesAndMoments,2);
        status[pos++] = gsl_vector_get(this->connectorsForcesAndMoments,3);
        status[pos++] = gsl_vector_get(this->connectorsForcesAndMoments,4);
        status[pos++] = gsl_vector_get(this->connectorsForcesAndMoments,5);

        // Output the Gravitational Forces and Moments
        status[pos++] = gsl_vector_get(gravitationalForceAndMoment,0);
        status[pos++] = gsl_vector_get(gravitationalForceAndMoment,1);
        status[pos++] = gsl_vector_get(gravitationalForceAndMoment,2);
        status[pos++] = gsl_vector_get(gravitationalForceAndMoment,3);
        status[pos++] = gsl_vector_get(gravitationalForceAndMoment,4);
        status[pos++] = gsl_vector_get(gravitationalForceAndMoment,5);

        // Output the Centrifugal Forces and Moments
        status[pos++] = gsl_vector_get(centrifugalForceAndMoment,0);
        status[pos++] = gsl_vector_get(centrifugalForceAndMoment,1);
        status[pos++] = gsl_vector_get(centrifugalForceAndMoment,2);
        status[pos++] = gsl_vector_get(centrifugalForceAndMoment,3);
        status[pos++] = gsl_vector_get(centrifugalForceAndMoment,4);
        status[pos++] = gsl_vector_get(centrifugalForceAndMoment,5);
    }

    const double* CarBodyComponent::getCurrentY(){
        return this->locY;
    }

    bool CarBodyComponent::loadStartingValues(XERCES_CPP_NAMESPACE::DOMElement* el, std::vector<double>& startingValues){
        bool errCode = false;

//        // Set X
//        errCode = XMLaux::loadDoubleProperty(el,"X",startingValues[this->get_startingDOF() + X]);
//        if (errCode) return errCode;
//        // Set XDOT
//        errCode = XMLaux::loadDoubleProperty(el,"XDOT",startingValues[this->get_startingDOF() + XDOT]);
        // Set Y
        errCode = XMLaux::loadDoubleProperty(el,"Y",startingValues[this->get_startingDOF() + Y]);
        if (errCode) return errCode;
        // Set YDOT
        errCode = XMLaux::loadDoubleProperty(el,"YDOT",startingValues[this->get_startingDOF() + YDOT]);
        if (errCode) return errCode;
        // Set Z
        errCode = XMLaux::loadDoubleProperty(el,"Z",startingValues[this->get_startingDOF() + Z]);
        if (errCode) return errCode;
        // Set ZDOT
        errCode = XMLaux::loadDoubleProperty(el,"ZDOT",startingValues[this->get_startingDOF() + ZDOT]);
        if (errCode) return errCode;
        // Set PSI
        errCode = XMLaux::loadDoubleProperty(el,"PSI",startingValues[this->get_startingDOF() + PSI]);
        if (errCode) return errCode;
        // Set PSIDOT
        errCode = XMLaux::loadDoubleProperty(el,"PSIDOT",startingValues[this->get_startingDOF() + PSIDOT]);
        if (errCode) return errCode;
        // Set PHI
        errCode = XMLaux::loadDoubleProperty(el,"PHI",startingValues[this->get_startingDOF() + PHI]);
        if (errCode) return errCode;
        // Set PHIDOT
        errCode = XMLaux::loadDoubleProperty(el,"PHIDOT",startingValues[this->get_startingDOF() + PHIDOT]);
        if (errCode) return errCode;
        // Set CHI
        errCode = XMLaux::loadDoubleProperty(el,"CHI",startingValues[this->get_startingDOF() + CHI]);
        if (errCode) return errCode;
        // Set CHIDOT
        errCode = XMLaux::loadDoubleProperty(el,"CHIDOT",startingValues[this->get_startingDOF() + CHIDOT]);
        if (errCode) return errCode;

        return errCode;
    }

    void CarBodyComponent::init(){
        this->rhs_counter = 0;
        this->getLowerLeading()->getComponent()->init();
        this->getLowerTrailing()->getComponent()->init();
    }

    std::string CarBodyComponent::printCounters(){
        std::stringstream out(std::stringstream::in|std::stringstream::out);
        out << this->get_name() << ": RHS evaluations \t" << this->rhs_counter << std::endl;
        out << this->getLowerLeading()->getComponent()->printCounters();
        out << this->getLowerTrailing()->getComponent()->printCounters();
        std::string outStr = out.str();
        return outStr;
    }

    int CarBodyComponent::computeFun( double t, double y[], double f[] ){
        int errCode = GSL_SUCCESS;

        struct ExecInput llexecInput;
        struct ExecInput ltexecInput;

        double* llyCopy;
        double* ltyCopy;

        /**
		 *  The following part has been moved to the parent class Component
		 */
//		// Start the lower components' function
//		if (Simulation::getExecMode() == NO_THREADS){
//			// Compute the lower leading component
//			errCode = this->getLowerLeading()->getComponent()->fun(t,y,f);
//			if ( errCode != GSL_SUCCESS )
//				return errCode;
//
//			// Compute function for the lower trailing component
//			errCode = this->getLowerTrailing()->getComponent()->fun(t,y,f);
//			if ( errCode != GSL_SUCCESS )
//				return errCode;
//		} else {
//			// Start the lower leading and lower trailing thread
//			// Pack input lower leading
//			llyCopy = new double[this->get_generalModel()->get_N_DOF()];
//			memcpy(llyCopy, y, this->get_generalModel()->get_N_DOF() * sizeof(double));
//			FunInput llfunInput = {t, llyCopy, f};
//			llexecInput.type = FUN;
//			llexecInput.input = &llfunInput;
//			// Pack input lower trailing
//			ltyCopy = new double[this->get_generalModel()->get_N_DOF()];
//			memcpy(ltyCopy, y, this->get_generalModel()->get_N_DOF() * sizeof(double));
//			FunInput ltfunInput = {t, ltyCopy, f};
//			ltexecInput.type = FUN;
//			ltexecInput.input = &ltfunInput;
//
//			this->getLowerLeading()->getComponent()->Start( &llexecInput );
//			this->getLowerTrailing()->getComponent()->Start( &ltexecInput );
//		}

        // Set up shortcuts
        this->llComponent = (DYTSI_Modelling::BogieFrameComponent*)
                this->getLowerLeading()->getComponent();
        this->llConnector = (DYTSI_Modelling::BogieFrameCarBodyConnector*)
                this->getLowerLeading()->getConnector();
        this->ltComponent = (DYTSI_Modelling::BogieFrameComponent*)
                this->getLowerTrailing()->getComponent();
        this->ltConnector = (DYTSI_Modelling::BogieFrameCarBodyConnector*)
                this->getLowerTrailing()->getConnector();

        // Set locF
        this->locF = &(f[this->get_startingDOF()]);

        // Compute the right hand side
        gsl_matrix_view rightHandSide_view = gsl_matrix_view_array( this->locF, this->N_DOF, 1 );
        //gsl_matrix* rightHandSide = &rightHandSide_view.matrix;
        errCode = this->rhs(t,y,&rightHandSide_view.matrix);
        if (errCode != GSL_SUCCESS){
            return errCode;
        }

        // Wait for the lower threads
        if (Simulation::getExecMode() == THREADS){
            this->getLowerLeading()->getComponent()->Join();
            this->getLowerTrailing()->getComponent()->Join();

            // Free Memory
            delete[] llyCopy;
            delete[] ltyCopy;

            // Check for errors
            if (llexecInput.returnCode != GSL_SUCCESS)
                return llexecInput.returnCode;
            if (ltexecInput.returnCode != GSL_SUCCESS)
                return ltexecInput.returnCode;
        }

        return GSL_SUCCESS;
    }

    int CarBodyComponent::computeJac( double t, double y[], double* dfdy, double dfdt[]){
        int errCode = GSL_SUCCESS;

        ExecInput llexecInput;
        ExecInput ltexecInput;
        
        double* llyCopy;
        double* ltyCopy;

        // Start the lower components' function
        if (Simulation::getExecMode() == NO_THREADS){
            // Compute jacobian for the lower leading component
            errCode = this->getLowerLeading()->getComponent()->jac(t,y,dfdy,dfdt);
            if ( errCode != GSL_SUCCESS )
                return errCode;

            // Compute jacobian for the lower trailing component
            errCode = this->getLowerTrailing()->getComponent()->jac(t,y,dfdy,dfdt);
            if ( errCode != GSL_SUCCESS )
                return errCode;
        } else {
            // Start the lower leading and lower trailing thread
            // Pack input lower leading
            llyCopy = new double[this->get_generalModel()->get_N_DOF()];
            memcpy(llyCopy, y, this->get_generalModel()->get_N_DOF() * sizeof(double));
            JacInput lljacInput = {t, llyCopy, dfdy, dfdt};
            llexecInput.type = JAC;
            llexecInput.input = &lljacInput;
            // Pack input lower trailing
            ltyCopy = new double[this->get_generalModel()->get_N_DOF()];
            memcpy(ltyCopy, y, this->get_generalModel()->get_N_DOF() * sizeof(double));
            JacInput ltjacInput = {t, ltyCopy, dfdy, dfdt};
            ltexecInput.type = JAC;
            ltexecInput.input = &ltjacInput;

            this->getLowerLeading()->getComponent()->Start( &llexecInput );
            this->getLowerTrailing()->getComponent()->Start( &ltexecInput );
        }

        // Set up shortcuts
        this->llComponent = (DYTSI_Modelling::BogieFrameComponent*)
                this->getLowerLeading()->getComponent();
        this->llConnector = (DYTSI_Modelling::BogieFrameCarBodyConnector*)
                this->getLowerLeading()->getConnector();
        this->ltComponent = (DYTSI_Modelling::BogieFrameComponent*)
                this->getLowerTrailing()->getComponent();
        this->ltConnector = (DYTSI_Modelling::BogieFrameCarBodyConnector*)
                this->getLowerTrailing()->getConnector();

        // Retreive the submatrix relative to the CarBody equations.
        int N_VAR = this->get_generalModel()->get_N_DOF();
        gsl_matrix_view dfdy_mat = gsl_matrix_view_array (dfdy,N_VAR,N_VAR);
        //gsl_matrix *MAT = &dfdy_mat.matrix;
        gsl_matrix_view sub_dfdy = gsl_matrix_submatrix(
                &dfdy_mat.matrix, this->get_startingDOF(), 0, this->get_N_DOF(), N_VAR);
        //gsl_matrix *J = &sub_dfdy.matrix;

        // Retreive the corresponding data in dfdt
        double *loc_dfdt = &(dfdt[this->get_startingDOF()]);
        // Derivatives w.r.t time
        for( int i = 0; i < this->get_N_DOF(); i++ ){
            loc_dfdt[i] = .0;
        }

        // Jacobian initialization
        gsl_matrix_set_zero(&sub_dfdy.matrix);

        // Jacobian: Numerical part
        for( std::vector<int>::iterator iter = this->partialJacobianIdx.begin();
                iter != this->partialJacobianIdx.end(); ++iter){
            errCode = this->jac_numerical(t,y,&sub_dfdy.matrix,*iter);
            if (errCode != GSL_SUCCESS)
                return errCode;
        }

        // Wait for the lower threads
        if (Simulation::getExecMode() == THREADS){
            this->getLowerLeading()->getComponent()->Join();
            this->getLowerTrailing()->getComponent()->Join();

            // Free Memory
            delete[] llyCopy;
            delete[] ltyCopy;

            // Check for errors
            if (llexecInput.returnCode != GSL_SUCCESS)
                return llexecInput.returnCode;
            if (ltexecInput.returnCode != GSL_SUCCESS)
                return ltexecInput.returnCode;
        }

        return errCode;
    }

    void CarBodyComponent::updateTransformationMatrix( gsl_matrix* mat, double y[]){
        gsl_matrix_set(mat, 0, 1, -y[this->get_startingDOF()+PSI]);
        gsl_matrix_set(mat, 1, 0, y[this->get_startingDOF()+PSI]);
        gsl_matrix_set(mat, 1, 2, -y[this->get_startingDOF()+PHI]);
        gsl_matrix_set(mat, 2, 1, y[this->get_startingDOF()+PHI]);
        gsl_matrix_set(mat, 0, 2, y[this->get_startingDOF()+CHI]);
        gsl_matrix_set(mat, 2, 0, -y[this->get_startingDOF()+CHI]);
    }

    void CarBodyComponent::updateDTransformationMatrix( gsl_matrix* mat, double y[]){
        gsl_matrix_set(mat, 0, 1, -y[this->get_startingDOF()+PSIDOT]);
        gsl_matrix_set(mat, 1, 0, y[this->get_startingDOF()+PSIDOT]);
        gsl_matrix_set(mat, 1, 2, -y[this->get_startingDOF()+PHIDOT]);
        gsl_matrix_set(mat, 2, 1, y[this->get_startingDOF()+PHIDOT]);
        gsl_matrix_set(mat, 0, 2, y[this->get_startingDOF()+CHIDOT]);
        gsl_matrix_set(mat, 2, 0, -y[this->get_startingDOF()+CHIDOT]);
    }

    void CarBodyComponent::updateDisplacementVector(gsl_vector* vec, double y[]){
//        gsl_vector_set(vec, 0, y[this->get_startingDOF()+X]);
        gsl_vector_set(vec, 0, 0.0);
        gsl_vector_set(vec, 1, y[this->get_startingDOF()+Y]);
        gsl_vector_set(vec, 2, y[this->get_startingDOF()+Z]);
    }

    void CarBodyComponent::updateDDisplacementVector(gsl_vector* vec, double y[]){
//        gsl_vector_set(vec, 0, y[this->get_startingDOF()+XDOT]);
        gsl_vector_set(vec, 0, 0.0);
        gsl_vector_set(vec, 1, y[this->get_startingDOF()+YDOT]);
        gsl_vector_set(vec, 2, y[this->get_startingDOF()+ZDOT]);
    }

    void CarBodyComponent::updateTransformationMatrix( gsl_matrix* mat, const double y[]){
        gsl_matrix_set(mat, 0, 1, -y[this->get_startingDOF()+PSI]);
        gsl_matrix_set(mat, 1, 0, y[this->get_startingDOF()+PSI]);
        gsl_matrix_set(mat, 1, 2, -y[this->get_startingDOF()+PHI]);
        gsl_matrix_set(mat, 2, 1, y[this->get_startingDOF()+PHI]);
        gsl_matrix_set(mat, 0, 2, y[this->get_startingDOF()+CHI]);
        gsl_matrix_set(mat, 2, 0, -y[this->get_startingDOF()+CHI]);
    }

    void CarBodyComponent::updateDTransformationMatrix( gsl_matrix* mat, const double y[]){
        gsl_matrix_set(mat, 0, 1, -y[this->get_startingDOF()+PSIDOT]);
        gsl_matrix_set(mat, 1, 0, y[this->get_startingDOF()+PSIDOT]);
        gsl_matrix_set(mat, 1, 2, -y[this->get_startingDOF()+PHIDOT]);
        gsl_matrix_set(mat, 2, 1, y[this->get_startingDOF()+PHIDOT]);
        gsl_matrix_set(mat, 0, 2, y[this->get_startingDOF()+CHIDOT]);
        gsl_matrix_set(mat, 2, 0, -y[this->get_startingDOF()+CHIDOT]);
    }

    void CarBodyComponent::updateDisplacementVector(gsl_vector* vec, const double y[]){
//        gsl_vector_set(vec, 0, y[this->get_startingDOF()+X]);
        gsl_vector_set(vec, 0, 0.0);
        gsl_vector_set(vec, 1, y[this->get_startingDOF()+Y]);
        gsl_vector_set(vec, 2, y[this->get_startingDOF()+Z]);
    }

    void CarBodyComponent::updateDDisplacementVector(gsl_vector* vec, const double y[]){
//        gsl_vector_set(vec, 0, y[this->get_startingDOF()+XDOT]);
        gsl_vector_set(vec, 0, 0.0);
        gsl_vector_set(vec, 1, y[this->get_startingDOF()+YDOT]);
        gsl_vector_set(vec, 2, y[this->get_startingDOF()+ZDOT]);
    }

    gsl_vector* CarBodyComponent::computeCentrifugalForceAndMoment( ){
        double val;

        // Get the nominal position of the body
        double l = gsl_vector_get(this->getCenterOfMass(),0);
        double b = gsl_vector_get(this->getCenterOfMass(),1);
        double h = gsl_vector_get(this->getCenterOfMass(),2);

        // Get the external components
        double v = this->getExternalComponents()->get_v();
        double vv = v*v;
        double R = this->getExternalComponents()->get_R();
        double RR = R*R;
        double sin_Phi_se = (this->getExternalComponents()->get_sin_Phi_se());
        double cos_Phi_se = (this->getExternalComponents()->get_cos_Phi_se());

//        // Set the x component
        gsl_vector_set(vecForceAndMoment,0,0.0);

        // Set the y component
//        val = this->get_mass() * (
//                cos_Phi_se * vv / R -
//                (h + this->locY[Z]) * sin_Phi_se * cos_Phi_se * vv / RR +
//                (b + this->locY[Y]) * cos_Phi_se * cos_Phi_se * vv / RR );
       val = this->get_mass() * cos_Phi_se * vv / R;
        gsl_vector_set(vecForceAndMoment,1,val);

        // Set the z component
//        val = this->get_mass() * (
//                - sin_Phi_se * vv / R +
//                (h + this->locY[Z]) * sin_Phi_se * sin_Phi_se * vv / RR -
//                (b + this->locY[Y]) * sin_Phi_se * cos_Phi_se * vv / RR);
        val = - this->get_mass() * sin_Phi_se *  vv / R;
        gsl_vector_set(vecForceAndMoment,2,val);

        // Set the phi component
//        val =   this->get_Ix() * (
//                    this->locY[PSIDOT] * sin_Phi_se * v / R -
//                    this->locY[CHIDOT] * cos_Phi_se * v / R) +
//                (this->get_Iy() - this->get_Iz()) * (
//                    sin_Phi_se * cos_Phi_se * vv / RR -
//                    this->locY[PSIDOT] * sin_Phi_se * v / R -
//                    this->locY[CHIDOT] * cos_Phi_se * v / R +
//                    this->locY[CHIDOT] * this->locY[PSIDOT] );
        val = 0.0;
        gsl_vector_set(vecForceAndMoment,3,val);

        // Set the chi component
//        val =   this->get_Iy() * this->locY[PHIDOT] * cos_Phi_se * v / R +
//                (this->get_Iz() - this->get_Ix()) * (
//                    -this->locY[PHIDOT] * cos_Phi_se * v / R -
//                    this->locY[PSIDOT] * this->locY[PHIDOT] );
        val = 0.0;
        gsl_vector_set(vecForceAndMoment,4,val);

        // Set the psi component
//        val =   this->get_Iz() * this->locY[PHIDOT] * sin_Phi_se * v / R +
//                (this->get_Ix() - this->get_Iy()) * (
//                    -this->locY[PHIDOT] * sin_Phi_se * v / R -
//                    this->locY[PHIDOT] * this->locY[CHIDOT]);
        val = 0.0;
        gsl_vector_set(vecForceAndMoment,5,val);

        return vecForceAndMoment;
    }

    int CarBodyComponent::computeConnectorsForcesAndMoments(){
        int errCode = GSL_SUCCESS;

        // Initialize the sum of forces and moments
        gsl_vector_set_zero(this->connectorsForcesAndMoments);

        /////////////////////////////////
        // Lower Leading connector
        {
            gsl_vector* fm = this->llConnector->getForcesAndMoments( LL ,
                    this->locDisp, this->llDisp, this->locDDisp, this->llDDisp,
                    this->locTransformationMatrix, this->llTransformationMatrix,
                    this->locDTransformationMatrix, this->llDTransformationMatrix,
                    errCode);
            if (errCode != GSL_SUCCESS){
                return errCode;
            }
            // Add to the total forces and moments
            gsl_blas_daxpy(1.0, fm, this->connectorsForcesAndMoments);
            // Free memory
            //gsl_vector_free(fm);
        }
        /////////////////////////////////

        /////////////////////////////////
        // Lower Trailing connector
        {
            gsl_vector* fm = this->ltConnector->getForcesAndMoments( LT,
                    this->locDisp, this->ltDisp, this->locDDisp, this->ltDDisp,
                    this->locTransformationMatrix, this->ltTransformationMatrix,
                    this->locDTransformationMatrix, this->ltDTransformationMatrix,
                    errCode);
            if (errCode != GSL_SUCCESS){
                return errCode;
            }
            // Add to the total forces and moments
            gsl_blas_daxpy(1.0, fm, this->connectorsForcesAndMoments);
            // Free memory
            //gsl_vector_free(fm);
            /////////////////////////////////

            return errCode;
        }
    }

    int CarBodyComponent::rhs(double t, const double y[], gsl_matrix* outRHS){
        int errCode = GSL_SUCCESS;
        this->rhs_counter++;
        std::stringstream* out;

        if (outRHS->size2 > 1){
            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << this->get_name() << ": Computation of the Right Hand Side: wrong size of the output matrix" << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            return -1;
        }

        // Set locY, llY, ltY and upY
        this->locY = &(y[this->get_startingDOF()]);
        this->llY = &(y[this->llComponent->get_startingDOF()]);
        this->ltY = &(y[this->ltComponent->get_startingDOF()]);

        // Update the transformation matrices
        this->updateTransformationMatrix(this->locTransformationMatrix, y);
        this->updateDTransformationMatrix(this->locDTransformationMatrix, y);
        this->llComponent->updateTransformationMatrix(this->llTransformationMatrix, y);
        this->llComponent->updateDTransformationMatrix(this->llDTransformationMatrix, y);
        this->ltComponent->updateTransformationMatrix(this->ltTransformationMatrix, y);
        this->ltComponent->updateDTransformationMatrix(this->ltDTransformationMatrix, y);

        // Update the displacement vectors
        this->updateDisplacementVector(this->locDisp, y);
        this->updateDDisplacementVector(this->locDDisp, y);
        this->llComponent->updateDisplacementVector(this->llDisp, y);
        this->llComponent->updateDDisplacementVector(this->llDDisp, y);
        this->ltComponent->updateDisplacementVector(this->ltDisp, y);
        this->ltComponent->updateDDisplacementVector(this->ltDDisp, y);

        // Compute the connectors forces
        errCode = this->computeConnectorsForcesAndMoments();
        if (errCode != GSL_SUCCESS){
            return errCode;
        }

        // Compute Gravitational forces and moments
        gsl_vector* gravitationalForceAndMoment = this->computeGravitationalForceAndMoment();

        // Compute Centrifugal forces and moments
        gsl_vector* centrifugalForceAndMoment = this->computeCentrifugalForceAndMoment();


//        // X
//        gsl_matrix_set(outRHS, X, 0, locY[XDOT]);
//
//        // XDOT
//        {
//            double out = ( gsl_vector_get(this->connectorsForcesAndMoments, 0) +
//                            gsl_vector_get(gravitationalForceAndMoment, 0) +
//                            gsl_vector_get(centrifugalForceAndMoment, 0)) / this->mass;
//            gsl_matrix_set(outRHS, XDOT, 0, out);
//        }

        // Y
        gsl_matrix_set(outRHS, Y, 0, locY[YDOT]);

        // YDOT
        {
            double out = ( gsl_vector_get(this->connectorsForcesAndMoments, 1) +
                            gsl_vector_get(gravitationalForceAndMoment, 1) +
                            gsl_vector_get(centrifugalForceAndMoment, 1)) / this->mass;
            gsl_matrix_set(outRHS, YDOT, 0, out);
        }

        // Z
        gsl_matrix_set(outRHS, Z, 0, locY[ZDOT]);

        // ZDOT
        {
            double out = ( gsl_vector_get(this->connectorsForcesAndMoments, 2) +
                            gsl_vector_get(gravitationalForceAndMoment, 2) +
                            gsl_vector_get(centrifugalForceAndMoment, 2)) / this->mass;
            gsl_matrix_set(outRHS, ZDOT, 0, out);
        }

        // PSI
        gsl_matrix_set(outRHS, PSI, 0, locY[PSIDOT]);

        // PSIDOT
        {
            double out = ( gsl_vector_get(this->connectorsForcesAndMoments, 5) +
                            gsl_vector_get(gravitationalForceAndMoment, 5) +
                            gsl_vector_get(centrifugalForceAndMoment, 5) ) / this->Iz;
            gsl_matrix_set(outRHS, PSIDOT, 0, out);
        }

        // PHI
        gsl_matrix_set(outRHS, PHI, 0, locY[PHIDOT]);

        // PHIDOT
        {
            double out = ( gsl_vector_get(this->connectorsForcesAndMoments, 3) +
                            gsl_vector_get(gravitationalForceAndMoment, 3) +
                            gsl_vector_get(centrifugalForceAndMoment, 3) ) / this->Ix;
            gsl_matrix_set(outRHS, PHIDOT, 0, out);
        }

        // CHI
        gsl_matrix_set(outRHS, CHI, 0, locY[CHIDOT]);

        // CHIDOT
        {
            double out = ( gsl_vector_get(this->connectorsForcesAndMoments, 4) +
                            gsl_vector_get(gravitationalForceAndMoment, 4) +
                            gsl_vector_get(centrifugalForceAndMoment, 4) ) / this->Ix;
            gsl_matrix_set(outRHS, CHIDOT, 0, out);
        }

        return errCode;
    } // End rhs
}
