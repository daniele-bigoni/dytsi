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
 * File:   WheelSetComponent.cpp
 * Author: bigo
 * 
 * Created on February 12, 2011, 4:01 PM
 */

#include "WheelSetComponent.h"
#include "ExternalComponents.h"
#include "RSGEOTable.h"
#include "ComponentConnectorCouple.h"
#include "WheelSetBogieFrameConnector.h"
#include "BogieFrameComponent.h"
#include "CarBodyComponent.h"
#include "GeneralModel.h"

#include "../Application/PrintingHandler.h"
#include "../Application/miscellaneous.h"
#include "../Application/Input/XMLaux.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

#include <math.h>
#include <sstream>
#include <stdio.h>
#include <vector>

const double g = 9.82;

const double MIN_RSGEO_NORMAL = 1.0;

namespace DYTSI_Modelling {
    const std::string* WheelSetComponent::DOF_Names[N_DOF] = {      //new std::string("X"), new std::string("XDOT"),
                                                                    new std::string("Y"), new std::string("YDOT"),
                                                                    new std::string("Z"), new std::string("ZDOT"),
                                                                    new std::string("PHI"), new std::string("PHIDOT"),
                                                                    new std::string("BETA"),
                                                                    new std::string("PSI"), new std::string("PSIDOT")};

    const std::string* WheelSetComponent::C_Forces_Names[2][3] =    {   {   new std::string("C-LEFT-FX"),
                                                                            new std::string("C-LEFT-FY"),
                                                                            new std::string("C-LEFT-FZ")},
                                                                        {   new std::string("C-RIGHT-FX"),
                                                                            new std::string("C-RIGHT-FY"),
                                                                            new std::string("C-RIGHT-FZ")} };

    const std::string* WheelSetComponent::N_Forces_Names[2][2] =    {   {   new std::string("N-LEFT-NY"),
                                                                            new std::string("N-LEFT-NZ")},
                                                                        {   new std::string("N-RIGHT-NY"),
                                                                            new std::string("N-RIGHT-NZ")} };

    WheelSetComponent::WheelSetComponent( ComponentPosition position,
            gsl_vector* centerOfGeometry,
            gsl_vector* centerOfMass,
            double mass,
            double Ix,
            double Iy,
            double Iz,
            double a,
            double mu,
            double G,
            ExternalComponents* externalComponents,
            std::vector<RSGEOTable*> rsgeo_table_list,
            std::string name,
            GeneralModel* gm,
            bool isFixed)
    : Component(name,gm, externalComponents, centerOfGeometry, centerOfMass, isFixed){
        this->setDOF(gm);

        this->position = position;
        this->mass = mass;
        this->Ix = Ix;
        this->Iy = Iy;
        this->Iz = Iz;
        this->a = a;
        this->mu = mu;
        this->G = G;
        this->rsgeo_table_list = rsgeo_table_list;

        // TODO consistent dimensions between RSGEO tables not checked
        // TODO The zero table is considered to be the one active at rest
        RSGEOTable* table = rsgeo_table_list[0]; 
        this->MAX_DISPL = table->get_MAX_DISPL();

        // Allocate structures for the Contact point computations
        doubleMatrixAllocSetZero(this->rsgeodata,2,table->get_N_COLS_RSGEO());
        doubleMatrixAllocSetZero(this->c_forces,2,3);
        doubleMatrixAllocSetZero(this->n_forces,2,2);
        doubleMatrixAllocSetZero(this->torques,2,3);

        // Set r0 the rolling radius at displacement 0
        this->r0 = table->getValue(DYTSI_Modelling::RSGEOTable::RSG_Kwz,0.0);

        // Allocation of the Transformation matrices
        this->locTransformationMatrix = gsl_matrix_calloc(3,3);
        for ( int i = 0; i < 3; i++ ) gsl_matrix_set(this->locTransformationMatrix,i,i,1.0);
        this->locDTransformationMatrix = gsl_matrix_calloc(3,3);
        this->upTransformationMatrix = gsl_matrix_calloc(3,3);
        for ( int i = 0; i < 3; i++ ) gsl_matrix_set(this->upTransformationMatrix,i,i,1.0);
        this->upDTransformationMatrix = gsl_matrix_calloc(3,3);
        // Allocation of the displacement vectors
        this->locDisp = gsl_vector_calloc(3);
        this->locDDisp = gsl_vector_calloc(3);
        this->upDisp = gsl_vector_calloc(3);
        this->upDDisp = gsl_vector_calloc(3);

        // Aux vectors
        this->vecForceAndMoment = gsl_vector_alloc(6);

        ///////////////////////////////////////////////////////
        // Set up interdependencies for the numerical Jacobian
        this->partialJacobianIdx.clear();

        // Local variables
//        this->partialJacobianIdx.push_back(this->get_startingDOF()+X);
//        this->partialJacobianIdx.push_back(this->get_startingDOF()+XDOT);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+Y);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+YDOT);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+Z);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+ZDOT);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+PHI);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+PHIDOT);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+BETA);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+PSI);
        this->partialJacobianIdx.push_back(this->get_startingDOF()+PSIDOT);
        /////////////////////////////

        this->recalc = true;
    }

    WheelSetComponent::~WheelSetComponent() {
    }

    std::vector<std::string> WheelSetComponent::getHeader(){
        std::vector<std::string> out;

        // Add degrees of freedom
        for( int i = 0; i < this->N_DOF; i++){
            std::stringstream* stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
            const std::string* str = this->DOF_Names[i];
            *stream << this->get_name() << "::" << *str;
            out.push_back(stream->str());
            delete stream;
        }

        // Add Contact forces
        for( int i = 0; i < 2; i++){
            for( int j = 0; j < 3; j++ ){
                std::stringstream* stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
                const std::string* str = this->C_Forces_Names[i][j];
                *stream << this->get_name() << "::" << *str;
                out.push_back(stream->str());
                delete stream;
            }
        }
        // Add Normal forces
        for( int i = 0; i < 2; i++){
            for( int j = 0; j < 2; j++ ){
                std::stringstream* stream = new std::stringstream(std::stringstream::in|std::stringstream::out);
                const std::string* str = this->N_Forces_Names[i][j];
                *stream << this->get_name() << "::" << *str;
                out.push_back(stream->str());
                delete stream;
            }
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

    void WheelSetComponent::getStatus(double t, double* y, double* status){
        // Output the DOF part
        memcpy(status, &y[this->get_startingDOF()], this->N_DOF * sizeof(double));

        ////////////////////////////////
        // Update all the forces
        int errCode = GSL_SUCCESS;

        // Set up shortcuts
        this->connector = (DYTSI_Modelling::WheelSetBogieFrameConnector*)this->getUpper()->getConnector();
        this->bogieFrame = (DYTSI_Modelling::BogieFrameComponent*)this->getUpper()->getComponent();

        // Set the locY and uY to the startingDOF of the current wheelset and the bogie frame
        this->locY = &(y[this->get_startingDOF()]);
        this->upY = &(y[this->bogieFrame->get_startingDOF()]);

        // Update the transformation matrices
        this->updateTransformationMatrix(this->locTransformationMatrix, y);
        this->updateDTransformationMatrix(this->locDTransformationMatrix, y);
        this->bogieFrame->updateTransformationMatrix(this->upTransformationMatrix, y);
        this->bogieFrame->updateDTransformationMatrix(this->upDTransformationMatrix, y);

        // Prepare the displacement vectors for the upper connector
        this->updateDisplacementVector(this->locDisp, y);
        this->updateDDisplacementVector(this->locDDisp, y);
        this->bogieFrame->updateDisplacementVector(this->upDisp, y);
        this->bogieFrame->updateDDisplacementVector(this->upDDisp, y);

        // Compute the contact forces
        errCode = this->find_contact_forces(t);

        // Compute the connectors forces
        errCode = this->computeConnectorsForcesAndMoments();

        // Compute Gravitational forces and moments
        gsl_vector* gravitationalForceAndMoment = this->computeGravitationalForceAndMoment();

        // Compute Centrifugal forces and moments
        gsl_vector* centrifugalForceAndMoment = this->computeCentrifugalForceAndMoment();
        ///////////////////////////////

        // Output the tangential forces forces
        int pos = this->get_N_DOF();
        for( int i = 0; i < 2; i++ ){
            for (int j = 0; j < 3; j++ ){
                status[pos] = this->c_forces[i][j];
                pos++;
            }
        }
        // Output the normal forces
        for( int i = 0; i < 2; i++ ){
            for (int j = 0; j < 2; j++ ){
                status[pos] = this->n_forces[i][j];
                pos++;
            }
        }

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

    const double* WheelSetComponent::getCurrentY(){
        return this->locY;
    }

    bool WheelSetComponent::loadStartingValues(XERCES_CPP_NAMESPACE::DOMElement* el, std::vector<double>& startingValues){
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
        // Set BETA
        errCode = XMLaux::loadDoubleProperty(el,"BETA",startingValues[this->get_startingDOF() + BETA]);
        if (errCode) return errCode;

        return errCode;
    }

    void WheelSetComponent::setConnection(Connector* connector,
                Component* component,
                RelativeComponentPosition position){
        // Call the super function
        super::setConnection(connector, component, position);

        DYTSI_Modelling::BogieFrameComponent *bf =
                (DYTSI_Modelling::BogieFrameComponent*) this->getUpper()->getComponent();
        DYTSI_Modelling::CarBodyComponent *cb =
                (DYTSI_Modelling::CarBodyComponent*) bf->getUpper()->getComponent();
        // Set this->mx
        this->mx = this->mass + 0.5 * bf->get_mass() + 0.25 * cb->get_mass();

    }

    void WheelSetComponent::init(){
        this->rhs_counter = 0;
    }

    std::string WheelSetComponent::printCounters(){
        std::stringstream out(std::stringstream::in|std::stringstream::out);
        out << this->get_name() << ": RHS evaluations \t" << this->rhs_counter << std::endl;
        std::string outStr = out.str();
        return outStr;
    }

    int WheelSetComponent::computeFun( double t, double y[], double f[] ){
        int errCode;

        // Set up shortcuts
        this->connector = (DYTSI_Modelling::WheelSetBogieFrameConnector*)this->getUpper()->getConnector();
        this->bogieFrame = (DYTSI_Modelling::BogieFrameComponent*)this->getUpper()->getComponent();

        // Set the locF to the startingDOF of the current wheelset
        this->locF = &(f[this->get_startingDOF()]);
        // Set the locY in order to detect derailment
        this->locY = &(y[this->get_startingDOF()]);

        // Check for derailment of the wheelset
        if ( fabs(locY[Y]) > MAX_DISPL ){
            std::stringstream out;
            out << this->get_name() << ": Derailment detected!" << std::endl;
            out << "Current displacement: " << locY[Y] << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDOUT);
            return GSL_EDOM;
        }

        // Compute the right hand side
        gsl_matrix_view rightHandSide_view = gsl_matrix_view_array( this->locF, this->N_DOF, 1 );
        //gsl_matrix* rightHandSide = &rightHandSide_view.matrix;
        errCode = this->rhs(t,y,&rightHandSide_view.matrix);
        if (errCode != GSL_SUCCESS){
            return errCode;
        }

        return GSL_SUCCESS;
    }

    int WheelSetComponent::computeJac( double t, double y[], double* dfdy, double dfdt[]){
        int errCode = GSL_SUCCESS;

        // Set up shortcuts
        this->connector = (DYTSI_Modelling::WheelSetBogieFrameConnector*)this->getUpper()->getConnector();
        this->bogieFrame = (DYTSI_Modelling::BogieFrameComponent*)this->getUpper()->getComponent();

        // Retreive the submatrix relative to the wheelset equations.
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
        for (std::vector<int>::iterator iter = this->partialJacobianIdx.begin();
                iter != this->partialJacobianIdx.end(); ++iter){
            errCode = this->jac_numerical(t,y,&sub_dfdy.matrix,*iter);
            if (errCode != GSL_SUCCESS)
                return errCode;
        }

        return GSL_SUCCESS;
    }

    void WheelSetComponent::updateTransformationMatrix( gsl_matrix* mat, const double y[]){
        gsl_matrix_set(mat, 0, 1, -y[this->get_startingDOF()+PSI]);
        gsl_matrix_set(mat, 1, 0, y[this->get_startingDOF()+PSI]);
        gsl_matrix_set(mat, 1, 2, -y[this->get_startingDOF()+PHI]);
        gsl_matrix_set(mat, 2, 1, y[this->get_startingDOF()+PHI]);
    }

    void WheelSetComponent::updateDTransformationMatrix( gsl_matrix* mat, const double y[]){
        gsl_matrix_set(mat, 0, 1, -y[this->get_startingDOF()+PSIDOT]);
        gsl_matrix_set(mat, 1, 0, y[this->get_startingDOF()+PSIDOT]);
        gsl_matrix_set(mat, 1, 2, -y[this->get_startingDOF()+PHIDOT]);
        gsl_matrix_set(mat, 2, 1, y[this->get_startingDOF()+PHIDOT]);
    }

    void WheelSetComponent::updateDisplacementVector(gsl_vector* vec, const double y[]){
//        gsl_vector_set(vec, 0, y[this->get_startingDOF()+X]);
        gsl_vector_set(vec, 0, 0.0);
        gsl_vector_set(vec, 1, y[this->get_startingDOF()+Y]);
        gsl_vector_set(vec, 2, y[this->get_startingDOF()+Z]);
    }

    void WheelSetComponent::updateDDisplacementVector(gsl_vector* vec, const double y[]){
//        gsl_vector_set(vec, 0, y[this->get_startingDOF()+XDOT]);
        gsl_vector_set(vec, 0, 0.0);
        gsl_vector_set(vec, 1, y[this->get_startingDOF()+YDOT]);
        gsl_vector_set(vec, 2, y[this->get_startingDOF()+ZDOT]);
    }

    void WheelSetComponent::updateTransformationMatrix( gsl_matrix* mat, double y[]){
        gsl_matrix_set(mat, 0, 1, -y[this->get_startingDOF()+PSI]);
        gsl_matrix_set(mat, 1, 0, y[this->get_startingDOF()+PSI]);
        gsl_matrix_set(mat, 1, 2, -y[this->get_startingDOF()+PHI]);
        gsl_matrix_set(mat, 2, 1, y[this->get_startingDOF()+PHI]);
    }

    void WheelSetComponent::updateDTransformationMatrix( gsl_matrix* mat, double y[]){
        gsl_matrix_set(mat, 0, 1, -y[this->get_startingDOF()+PSIDOT]);
        gsl_matrix_set(mat, 1, 0, y[this->get_startingDOF()+PSIDOT]);
        gsl_matrix_set(mat, 1, 2, -y[this->get_startingDOF()+PHIDOT]);
        gsl_matrix_set(mat, 2, 1, y[this->get_startingDOF()+PHIDOT]);
    }

    void WheelSetComponent::updateDisplacementVector(gsl_vector* vec, double y[]){
//        gsl_vector_set(vec, 0, y[this->get_startingDOF()+X]);
        gsl_vector_set(vec, 0, 0.0);
        gsl_vector_set(vec, 1, y[this->get_startingDOF()+Y]);
        gsl_vector_set(vec, 2, y[this->get_startingDOF()+Z]);
    }

    void WheelSetComponent::updateDDisplacementVector(gsl_vector* vec, double y[]){
//        gsl_vector_set(vec, 0, y[this->get_startingDOF()+XDOT]);
        gsl_vector_set(vec, 0, 0.0);
        gsl_vector_set(vec, 1, y[this->get_startingDOF()+YDOT]);
        gsl_vector_set(vec, 2, y[this->get_startingDOF()+ZDOT]);
    }

    int WheelSetComponent::computeConnectorsForcesAndMoments(){
        int errCode = GSL_SUCCESS;

        // Initialize the sum of forces and moments
        gsl_vector_set_zero(this->connectorsForcesAndMoments);

        // Computation of forces and moments for the upper connector
        gsl_vector* fm = this->connector->getForcesAndMoments( UP,
                this->locDisp, this->upDisp, this->locDDisp, this->upDDisp,
                this->locTransformationMatrix, this->upTransformationMatrix,
                this->locDTransformationMatrix, this->upDTransformationMatrix,
                errCode);
        if (errCode != GSL_SUCCESS){
            return errCode;
        }

        // Add to the total forces and moments
        gsl_blas_daxpy(1.0, fm, this->connectorsForcesAndMoments);

        //gsl_vector_free(fm);

        return errCode;
    }

    gsl_vector* WheelSetComponent::computeCentrifugalForceAndMoment( ){
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

        double Omega = v/this->r0 + this->locY[BETA];

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
        val =   this->get_Ix() * (
                     - Omega * cos_Phi_se * v / R) +
                (this->get_Iy() - this->get_Iz()) * (
                     - Omega * cos_Phi_se * v / R +
                    Omega * this->locY[PSIDOT] );
//        val = 0.0;
        gsl_vector_set(vecForceAndMoment,3,val);

        // Set the chi component
//        val =   this->get_Iy() * this->locY[PHIDOT] * cos_Phi_se * v / R +
//                (this->get_Iz() - this->get_Ix()) * (
//                    -this->locY[PHIDOT] * cos_Phi_se * v / R -
//                    this->locY[PSIDOT] * this->locY[PHIDOT] );
        val = 0.0;
        gsl_vector_set(vecForceAndMoment,4,val);

        // Set the psi component
        val =   (this->get_Ix() - this->get_Iy()) * (
                     this->locY[PHIDOT] * Omega);
//        val = 0.0;
        gsl_vector_set(vecForceAndMoment,5,val);

        return vecForceAndMoment;
    }

    int WheelSetComponent::find_contact_forces(double t){
        int errCode = GSL_SUCCESS;

        if (this->recalc){
            this->c_forces[LEFT][FX] = .0;
            this->c_forces[LEFT][FY] = .0;
            this->c_forces[LEFT][FZ] = .0;
            this->c_forces[RIGHT][FX] = .0;
            this->c_forces[RIGHT][FY] = .0;
            this->c_forces[RIGHT][FZ] = .0;
            this->n_forces[LEFT][NY] = .0;
            this->n_forces[LEFT][NZ] = .0;
            this->n_forces[RIGHT][NY] = .0;
            this->n_forces[RIGHT][NZ] = .0;
            this->torques[LEFT][TPHI] = .0;
            this->torques[LEFT][TCHI] = .0;
            this->torques[LEFT][TPSI] = .0;
            this->torques[RIGHT][TPHI] = .0;
            this->torques[RIGHT][TCHI] = .0;
            this->torques[RIGHT][TPSI] = .0;

            // For each table available (for each contact point on the wheel)
            for (std::vector<RSGEOTable*>::iterator itRSGEO = this->rsgeo_table_list.begin(); itRSGEO != this->rsgeo_table_list.end(); ++itRSGEO) {
                RSGEOTable* rsg = *itRSGEO;

                double N[2];
                double delta[2];
                double cforces[2];
                double Fcreepx;
                double Fcreepy;
                double psi; // Wheelset yaw angle
                double phi; // Wheelset roll angle

                errCode = this->find_rsgeodata(t,rsg);
                if (errCode != GSL_SUCCESS){
                    return errCode;
                }

                psi = this->locY[PSI];
                phi = this->locY[PHI];

                N[LEFT] = this->rsgeodata[LEFT][DYTSI_Modelling::RSGEOTable::RSG_N];
                N[RIGHT] = this->rsgeodata[RIGHT][DYTSI_Modelling::RSGEOTable::RSG_N];

                // If there are contact point data on the current table
                if (N[LEFT] > MIN_RSGEO_NORMAL){
                    delta[LEFT] = this->rsgeodata[LEFT][DYTSI_Modelling::RSGEOTable::RSG_angle];
                    // Creep forces LEFT
                    errCode = this->SHE(t,LEFT,cforces);
                    if (errCode != GSL_SUCCESS)
                        return errCode;
                    Fcreepx = cforces[0];
                    Fcreepy = cforces[1];
                    double Flx = Fcreepx-Fcreepy*psi*cos(delta[LEFT]);
                    double Fly = Fcreepx*psi + Fcreepy*cos(delta[LEFT]+phi);
                    double Flz = Fcreepy*sin(delta[LEFT]+phi);
                    this->c_forces[LEFT][FX] += Flx;
                    this->c_forces[LEFT][FY] += Fly;
                    this->c_forces[LEFT][FZ] += Flz;
                    // Normal forces LEFT
                    double Nly = -N[LEFT]*sin(delta[LEFT]+phi);
                    double Nlz = N[LEFT]*cos(delta[LEFT]+phi);
                    this->n_forces[LEFT][NY] += Nly;
                    this->n_forces[LEFT][NZ] += Nlz;
                    // Torques
                    double al = this->rsgeodata[LEFT][RSGEOTable::RSG_Kwy];
                    double rl = this->rsgeodata[LEFT][RSGEOTable::RSG_Kwz];
                    this->torques[LEFT][TPHI] += al * (Flz+Nlz -(Fly + Nly)*phi);
                    this->torques[LEFT][TCHI] += - rl * (Flx + (Fly + Nly)*psi);
                    this->torques[LEFT][TPSI] += - al * (Flx + (Fly + Nly)*psi);
                }

                // If there are contact point data in the current table
                if (N[RIGHT] > MIN_RSGEO_NORMAL){
                    delta[RIGHT] = this->rsgeodata[RIGHT][DYTSI_Modelling::RSGEOTable::RSG_angle];
                    // Creep forces RIGHT
                    errCode = this->SHE(t,RIGHT,cforces);
                    if (errCode != GSL_SUCCESS)
                        return errCode;
                    Fcreepx = cforces[0];
                    Fcreepy = cforces[1];
                    double Frx = Fcreepx-Fcreepy*psi*cos(delta[RIGHT]);
                    double Fry = Fcreepx*psi + Fcreepy*cos(delta[RIGHT]-phi);
                    double Frz = -Fcreepy*sin(delta[RIGHT]-phi);
                    this->c_forces[RIGHT][FX] += Frx;
                    this->c_forces[RIGHT][FY] += Fry;
                    this->c_forces[RIGHT][FZ] += Frz;
                    // Normal forces RIGHT
                    double Nry = N[RIGHT]*sin(delta[RIGHT]-phi);
                    double Nrz = N[RIGHT]*cos(delta[RIGHT]-phi);
                    this->n_forces[RIGHT][NY] += Nry;
                    this->n_forces[RIGHT][NZ] += Nrz;
                    // Torques
                    double ar = this->rsgeodata[RIGHT][RSGEOTable::RSG_Kwy];
                    double rr = this->rsgeodata[RIGHT][RSGEOTable::RSG_Kwz];
                    this->torques[RIGHT][TPHI] += -ar* (Frz+Nrz -(Fry + Nry)*phi);
                    this->torques[RIGHT][TCHI] += -rr * (Frx + (Fry + Nry)*psi);
                    this->torques[RIGHT][TPSI] += ar * (Frx + (Fry + Nry)*locY[PSI]);
                }
            }

            // Check for NaN values
            if (    isnan(this->c_forces[LEFT][FX]) || isnan(this->c_forces[LEFT][FY]) || isnan(this->c_forces[LEFT][FZ]) ||
                    isnan(this->c_forces[RIGHT][FX]) || isnan(this->c_forces[RIGHT][FY]) || isnan(this->c_forces[RIGHT][FZ]) ||
                    isnan(this->n_forces[LEFT][NY]) || isnan(this->n_forces[LEFT][NZ]) ||
                    isnan(this->n_forces[RIGHT][NY]) || isnan(this->n_forces[RIGHT][NZ]) ||
                    isnan(this->torques[LEFT][TPHI]) || isnan(this->torques[LEFT][TCHI]) || isnan(this->torques[LEFT][TPSI]) ||
                    isnan(this->torques[RIGHT][TPHI]) || isnan(this->torques[RIGHT][TCHI]) || isnan(this->torques[RIGHT][TPSI]) ){
                errCode = GSL_EDOM;
                std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
                *out << "Error: computation of Contact-Normal forces gives NaN values for " << this->get_name() << std::endl;
                *out << "Current wheel displacements:" << std::endl;
                for (int i = 0; i < this->get_N_DOF(); i++){
                    *out << this->locY[i] << " ";
                }
                *out << std::endl;
                PrintingHandler::printOut(out,PrintingHandler::STDOUT);
                delete out;
            }

        }
        
        return errCode;
    }

    int WheelSetComponent::find_rsgeodata(double t, RSGEOTable* rsg){
        double lat_disp[2];
        double roll[2];
        double N[2];
        double rollRad[2];
        double cpDist[2];

        if ( fabs(locY[Y]) > MAX_DISPL ){
            std::stringstream out;
            out << this->get_name() << ": Derailment detected!" << std::endl;
            out << "Current displacement: " << locY[Y] << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDOUT);
            return GSL_EDOM;
        }
        lat_disp[LEFT] = this->locY[Y];
        lat_disp[RIGHT] = - this->locY[Y];

        roll[LEFT] = this->locY[PHI];
        roll[RIGHT] = - this->locY[PHI];

        // Interpolation performed in class DYTSI_Modelling::RSGEOTable
        this->rsgeodata[LEFT][0] = lat_disp[LEFT];
        this->rsgeodata[RIGHT][0] = lat_disp[RIGHT];
        for( int i = 1; i < rsg->get_N_COLS_RSGEO(); i++ ){
            rsgeodata[LEFT][i] = rsg->getValue((RSGEOTable::rsgeo_values)i,lat_disp[LEFT]);
            rsgeodata[RIGHT][i] = rsg->getValue((RSGEOTable::rsgeo_values)i,lat_disp[RIGHT]);
        }

        N[LEFT] = rsgeodata[LEFT][DYTSI_Modelling::RSGEOTable::RSG_N];
        N[RIGHT] = rsgeodata[RIGHT][DYTSI_Modelling::RSGEOTable::RSG_N];
        if (N[LEFT] > MIN_RSGEO_NORMAL){
            rollRad[LEFT] = rsgeodata[LEFT][DYTSI_Modelling::RSGEOTable::RSG_Kwz];
            cpDist[LEFT] = rsgeodata[LEFT][DYTSI_Modelling::RSGEOTable::RSG_Kwy];
            rsgeodata[LEFT][DYTSI_Modelling::RSGEOTable::RSG_Kry] =
                (rollRad[LEFT]-tan(roll[LEFT])*cpDist[LEFT])*sin(roll[LEFT]) +
                cpDist[LEFT]/cos(roll[LEFT]) + lat_disp[LEFT];
            this->dynamic_update(t,LEFT);
        }
        if (N[RIGHT] > MIN_RSGEO_NORMAL){
            rollRad[RIGHT] = rsgeodata[RIGHT][DYTSI_Modelling::RSGEOTable::RSG_Kwz];
            cpDist[RIGHT] = rsgeodata[RIGHT][DYTSI_Modelling::RSGEOTable::RSG_Kwy];
            rsgeodata[RIGHT][DYTSI_Modelling::RSGEOTable::RSG_Kry] =
                (rollRad[RIGHT]-tan(roll[RIGHT])*cpDist[RIGHT])*sin(roll[RIGHT]) +
                cpDist[RIGHT]/cos(roll[RIGHT]) + lat_disp[RIGHT];
            this->dynamic_update(t,RIGHT);
        }

        return GSL_SUCCESS;
    }

    int WheelSetComponent::SHE(double t, int wheel, double *out){
        double norm_F_tau; // 2-norm of tangential force on the contact plane
        double epsilon; // Reduction coeff.
        double Fx_val; // Creep force wrt the contact system
        double Fy_val; // Creep force wrt the contact system
        double cp_a, cp_b; // Semi axes of the contact ellipse
        double C11, C22, C23; // Kalker's creepage coeff.
        double xi_x, xi_y, xi_s; // Longitudinal, Lateral and Spin creep
        double radius; // Rolling radius
        double aw; // Distance from the contact point
        double delta; // Angle wheel axle-contact plane
        double N; // Normal force

        // Load data from RSGEO
        N = rsgeodata[wheel][DYTSI_Modelling::RSGEOTable::RSG_N];
        cp_a = rsgeodata[wheel][DYTSI_Modelling::RSGEOTable::RSG_a];
        cp_b = rsgeodata[wheel][DYTSI_Modelling::RSGEOTable::RSG_b];
        C11 = rsgeodata[wheel][DYTSI_Modelling::RSGEOTable::RSG_C11];
        C22 = rsgeodata[wheel][DYTSI_Modelling::RSGEOTable::RSG_C22];
        C23 = rsgeodata[wheel][DYTSI_Modelling::RSGEOTable::RSG_C23];
        radius = rsgeodata[wheel][DYTSI_Modelling::RSGEOTable::RSG_Kwz];
        aw = rsgeodata[wheel][DYTSI_Modelling::RSGEOTable::RSG_Kwy];
        delta = rsgeodata[wheel][DYTSI_Modelling::RSGEOTable::RSG_angle];

        switch (this->position){
            case LEADING:
                switch (wheel){
                case LEFT:
                    xi_x = xi_flx(locY[PSI],locY[PSIDOT],locY[PHIDOT],locY[YDOT],locY[BETA], radius, aw);
                    xi_y = xi_fly(locY[PSI],locY[PHI],locY[PHIDOT],locY[YDOT],locY[ZDOT], radius, aw, delta);
                    xi_s = xi_fls(locY[PSI],locY[PSIDOT],locY[PHIDOT],locY[BETA], delta);
                  break;
                case RIGHT:
                    xi_x = xi_frx(locY[PSI],locY[PSIDOT],locY[PHIDOT],locY[YDOT],locY[BETA], radius, aw);
                    xi_y = xi_fry(locY[PSI],locY[PHI],locY[PHIDOT],locY[YDOT],locY[ZDOT], radius, aw, delta);
                    xi_s = xi_frs(locY[PSI],locY[PSIDOT],locY[PHIDOT],locY[BETA], delta);
                  break;
                }
                break;

            case TRAILING:
                switch (wheel){
                case LEFT:
                    xi_x = xi_rlx(locY[PSI],locY[PSIDOT],locY[PHIDOT],locY[YDOT],locY[BETA], radius, aw);
                    xi_y = xi_rly(locY[PSI],locY[PHI],locY[PHIDOT],locY[YDOT],locY[ZDOT], radius, aw, delta);
                    xi_s = xi_rls(locY[PSI],locY[PSIDOT],locY[PHIDOT],locY[BETA], delta);
                  break;
                case RIGHT:
                    xi_x = xi_rrx(locY[PSI],locY[PSIDOT],locY[PHIDOT],locY[YDOT],locY[BETA], radius, aw);
                    xi_y = xi_rry(locY[PSI],locY[PHI],locY[PHIDOT],locY[YDOT],locY[ZDOT], radius, aw, delta);
                    xi_s = xi_rrs(locY[PSI],locY[PSIDOT],locY[PHIDOT],locY[BETA], delta);
                  break;
                }
                break;
        } // end switch

        if (N == 0){
            out[0] = 0;
            out[1] = 0;
        } else {
            Fx_val = Fx(cp_a,cp_b,C11,xi_x);
            Fy_val = Fy(cp_a,cp_b,C22,C23,xi_y,xi_s);
            double norm_F_tau_ping = sqrt(pow(Fx_val,2.0)+pow(Fy_val,2.0));
            double u = (norm_F_tau_ping/(this->mu*N));
            if (u<3)
              norm_F_tau = this->mu*N*(u - 1.0/3.0*pow(u,2.0) + 1.0/27.0*pow(u,3.0));
            else
              norm_F_tau = this->mu*N;
            epsilon = norm_F_tau / norm_F_tau_ping;
            out[0] = epsilon*Fx_val;
            out[1] = epsilon*Fy_val;
        }

        return GSL_SUCCESS;
    } // End SHE

    int WheelSetComponent::rhs(double t, const double y[], gsl_matrix* outRHS){
        int errCode = GSL_SUCCESS;
        this->rhs_counter++;
        std::stringstream* out;

        if (outRHS->size2 > 1){
            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << this->get_name() << ": Computation of the Right Hand Side: wrong size of the output matrix" << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            return -1;
        }

        // Set the locY and uY to the startingDOF of the current wheelset and the bogie frame
        this->locY = &(y[this->get_startingDOF()]);
        this->upY = &(y[this->bogieFrame->get_startingDOF()]);

        // Update the transformation matrices
        this->updateTransformationMatrix(this->locTransformationMatrix, y);
        this->updateDTransformationMatrix(this->locDTransformationMatrix, y);
        this->bogieFrame->updateTransformationMatrix(this->upTransformationMatrix, y);
        this->bogieFrame->updateDTransformationMatrix(this->upDTransformationMatrix, y);

        // Prepare the displacement vectors for the upper connector
        this->updateDisplacementVector(this->locDisp, y);
        this->updateDDisplacementVector(this->locDDisp, y);
        this->bogieFrame->updateDisplacementVector(this->upDisp, y);
        this->bogieFrame->updateDDisplacementVector(this->upDDisp, y);

        // Check for derailment
        // Check for derailment of the wheelset
        if ( fabs(locY[Y]) > MAX_DISPL ){
            out = new std::stringstream(std::stringstream::in|std::stringstream::out);
            *out << this->get_name() << ": Derailment detected!" << std::endl;
            *out << "Current displacement: " << locY[Y] << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDOUT);
            return GSL_EDOM;
        }

        // Compute the contact forces
        errCode = this->find_contact_forces(t);
        if (errCode != GSL_SUCCESS){
            return errCode;
        }

        // Compute the connectors forces
        errCode = this->computeConnectorsForcesAndMoments();
        if (errCode != GSL_SUCCESS){
            return errCode;
        }

        // Compute Gravitational forces and moments
        gsl_vector* gravitationalForceAndMoment = this->computeGravitationalForceAndMoment();

        // Compute Centrifugal forces and moments
        gsl_vector* centrifugalForceAndMoment = this->computeCentrifugalForceAndMoment();

        using namespace DYTSI_Modelling;

//        // X
//        gsl_matrix_set(outRHS,X,0,locY[XDOT]);
//
//        // XDOT
//        {
//            double Frx = this->c_forces[RIGHT][FX];
//            double Flx = this->c_forces[LEFT][FX];
//
//            double out = (Frx + Flx +
//                            gsl_vector_get(this->connectorsForcesAndMoments,0) +
//                            gsl_vector_get(gravitationalForceAndMoment, 0) +
//                            gsl_vector_get(centrifugalForceAndMoment, 0)) / this->mass;
//            gsl_matrix_set(outRHS,XDOT,0,out);
//        }

        // Y
        gsl_matrix_set(outRHS, Y, 0, locY[YDOT]);

        // YDOT
        {
            double Fly = this->c_forces[LEFT][FY];
            double Fry = this->c_forces[RIGHT][FY];
            double Nly = this->n_forces[LEFT][NY];
            double Nry = this->n_forces[RIGHT][NY];

            double out = (Fly + Fry + Nly + Nry + 
                            gsl_vector_get(this->connectorsForcesAndMoments, 1) +
                            gsl_vector_get(gravitationalForceAndMoment, 1) +
                            gsl_vector_get(centrifugalForceAndMoment, 1)) / this->mass;
            gsl_matrix_set(outRHS, YDOT, 0, out);
        }

        // Z
        gsl_matrix_set(outRHS, Z, 0, locY[ZDOT]);

        // ZDOT
        {
            double Flz = this->c_forces[LEFT][FZ];
            double Frz = this->c_forces[RIGHT][FZ];
            double Nlz = this->n_forces[LEFT][NZ];
            double Nrz = this->n_forces[RIGHT][NZ];

            double out = (Flz + Frz + Nlz + Nrz + 
                            gsl_vector_get(this->connectorsForcesAndMoments, 2) +
                            gsl_vector_get(gravitationalForceAndMoment, 2) +
                            gsl_vector_get(centrifugalForceAndMoment, 2)) / this->mass;
            gsl_matrix_set(outRHS, ZDOT, 0, out);
        }

        // PSI
        gsl_matrix_set(outRHS, PSI, 0, locY[PSIDOT]);

        // PSIDOT
        {
            double Tr = this->torques[RIGHT][TPSI];
            double Tl = this->torques[LEFT][TPSI];

            double out = ( Tr + Tl +
                            gsl_vector_get(this->connectorsForcesAndMoments, 5) +
                            gsl_vector_get(gravitationalForceAndMoment, 5) +
                            gsl_vector_get(centrifugalForceAndMoment, 5) ) / this->Iz;
            gsl_matrix_set(outRHS, PSIDOT, 0, out);
        }

        // PHI
        gsl_matrix_set(outRHS, PHI, 0, locY[PHIDOT]);

        // PHIDOT
        {
            double Tr = this->torques[RIGHT][TPHI];
            double Tl = this->torques[LEFT][TPHI];

            double out = ( Tr + Tl +
                            gsl_vector_get(this->connectorsForcesAndMoments, 3) +
                            gsl_vector_get(gravitationalForceAndMoment, 3) +
                            gsl_vector_get(centrifugalForceAndMoment, 3) ) / this->Ix;
            gsl_matrix_set(outRHS, PHIDOT, 0, out);
        };

        // BETA
        {
            double Tr = this->torques[RIGHT][TCHI];
            double Tl = this->torques[LEFT][TCHI];
            
            double out = ( Tr + Tl ) / this->Iy;
            gsl_matrix_set(outRHS, BETA, 0, out);
        };

        return errCode;

    }

    void WheelSetComponent::dynamic_update( double t, int wheel ){
        double y_lat, z_ver, phi;

        y_lat = this->locY[Y];
        z_ver = this->locY[Z];
        phi = this->locY[PHI];

        switch (wheel) {
            case LEFT:
                this->dynamic_aux( this->rsgeodata[LEFT][RSGEOTable::RSG_Kwz], this->rsgeodata[LEFT][RSGEOTable::RSG_Kwy],
                    this->rsgeodata[LEFT][RSGEOTable::RSG_Kry], this->rsgeodata[LEFT][RSGEOTable::RSG_Krz],
                    this->rsgeodata[LEFT][RSGEOTable::RSG_angle], y_lat, z_ver, phi, LEFT);
                break;
            case RIGHT:
                this->dynamic_aux( this->rsgeodata[RIGHT][RSGEOTable::RSG_Kwz], this->rsgeodata[RIGHT][RSGEOTable::RSG_Kwy],
                    this->rsgeodata[RIGHT][RSGEOTable::RSG_Kry], this->rsgeodata[RIGHT][RSGEOTable::RSG_Krz],
                    this->rsgeodata[RIGHT][RSGEOTable::RSG_angle], y_lat, z_ver, phi, RIGHT);
                break;
        }
    }

    void WheelSetComponent::dynamic_aux( double radius, double aw, double at, double zt, double delta,
            double y_lat, double z_ver, double phi, int wheel){
        double dq, N, N0, N3, q0;

        switch (wheel){
            case LEFT:
                dq = -(at-y_lat-aw-phi*radius)*sin(delta+phi)+(-z_ver-phi*aw)*cos(delta+phi);
                break;
            case RIGHT:
                dq = (-at-y_lat+aw-phi*radius)*sin(delta-phi)+(-z_ver+phi*aw)*cos(delta-phi);
                break;
        }

        N0 = this->rsgeodata[wheel][RSGEOTable::RSG_N];
        q0 = this->rsgeodata[wheel][RSGEOTable::RSG_qN];
        N = N0*pow(1+dq/q0,1.5); if (isnan(N)) N = 0;
        N3 = pow(N/N0,1.0/3.0);
        this->rsgeodata[wheel][RSGEOTable::RSG_a] *= N3;
        this->rsgeodata[wheel][RSGEOTable::RSG_b] *= N3;
        this->rsgeodata[wheel][RSGEOTable::RSG_N] = N;
    }

    inline double WheelSetComponent::xi_flx(double psi, double psidot, double phidot,
            double ydot, double beta, double rl, double al){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double phi_se = this->getExternalComponents()->get_Phi_se();
        double b = this->bogieFrame->get_b();
        double a = this->get_a();
        double Omega_0 = v/this->r0;
        return (v + (psi+b/R)*ydot + rl*((psi+b/R)*phidot - (Omega_0+beta)) -
                al*psidot + a*v*cos(phi_se)/R)/v;
    }

    inline double WheelSetComponent::xi_frx(double psi, double psidot, double phidot,
            double ydot, double beta, double rr, double ar){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double phi_se = this->getExternalComponents()->get_Phi_se();
        double b = this->bogieFrame->get_b();
        double a = this->get_a();
        double Omega_0 = v/this->r0;
        return (v + (psi+b/R)*ydot + rr*((psi+b/R)*phidot - (Omega_0+beta)) +
                ar*psidot - a*v*cos(phi_se)/R)/v;
    }

    inline double WheelSetComponent::xi_fly(double psi, double phi, double phidot, 
            double ydot, double zdot, double rl, double al, double deltal){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double phi_se = this->getExternalComponents()->get_Phi_se();
        double b = this->bogieFrame->get_b();
        double a = this->get_a();
        return ((-v*(psi+b/R)*(1+a*cos(phi_se)/R)+phi*zdot+ydot+phidot*rl) * cos(deltal) +
                (-phi*ydot+zdot+al*phidot)*sin(deltal))/v;
    }

    inline double WheelSetComponent::xi_fry(double psi, double phi, double phidot, 
            double ydot, double zdot, double rr, double ar, double deltar){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double phi_se = this->getExternalComponents()->get_Phi_se();
        double b = this->bogieFrame->get_b();
        double a = this->get_a();
        return ((-v*(psi+b/R)*(1-a*cos(phi_se)/R)+phi*zdot+ydot+phidot*rr) * cos(deltar) -
                (-phi*ydot+zdot-ar*phidot)*sin(deltar))/v;
    }

    inline double WheelSetComponent::xi_rlx(double psi, double psidot, double phidot,
            double ydot, double beta, double rl, double al){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double phi_se = this->getExternalComponents()->get_Phi_se();
        double b = this->bogieFrame->get_b();
        double a = this->get_a();
        double Omega_0 = v/this->r0;
        return (v + (psi-b/R)*ydot + rl*((psi-b/R)*phidot- (Omega_0+beta)) -
                al*psidot + a*v*cos(phi_se)/R)/v;
    }

    inline double WheelSetComponent::xi_rrx(double psi, double psidot, double phidot,
            double ydot, double beta, double rr, double ar){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double phi_se = this->getExternalComponents()->get_Phi_se();
        double b = this->bogieFrame->get_b();
        double a = this->get_a();
        double Omega_0 = v/this->r0;
        return (v + (psi-b/R)*ydot + rr*((psi-b/R)*phidot- (Omega_0+beta)) +
                ar*psidot - a*v*cos(phi_se)/R)/v;
    }

    inline double WheelSetComponent::xi_rly(double psi, double phi, double phidot,
            double ydot, double zdot, double rl, double al, double deltal){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double phi_se = this->getExternalComponents()->get_Phi_se();
        double b = this->bogieFrame->get_b();
        double a = this->get_a();
        return ((-v*(psi-b/R)*(1+a*cos(phi_se)/R)+phi*zdot+ydot+phidot*rl) * cos(deltal) +
                (-phi*ydot+zdot+al*phidot)*sin(deltal))/v;
    }

    inline double WheelSetComponent::xi_rry(double psi, double phi, double phidot,
            double ydot, double zdot, double rr, double ar, double deltar){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double phi_se = this->getExternalComponents()->get_Phi_se();
        double b = this->bogieFrame->get_b();
        double a = this->get_a();
        return ((-v*(psi-b/R)*(1-a*cos(phi_se)/R)+phi*zdot+ydot+phidot*rr) * cos(deltar) -
                (-phi*ydot+zdot-ar*phidot)*sin(deltar))/v;
    }

    inline double WheelSetComponent::xi_fls(double psi, double psidot, double phidot, 
            double beta, double deltal){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double b = this->bogieFrame->get_b();
        double Omega_0 = v/this->r0;
        return (-(Omega_0+beta-(psi+b/R)*phidot)*sin(deltal) + psidot*cos(deltal))/v;
    }

    inline double WheelSetComponent::xi_frs(double psi, double psidot, double phidot,
            double beta, double deltar){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double b = this->bogieFrame->get_b();
        double Omega_0 = v/this->r0;
        return ((Omega_0+beta-(psi+b/R)*phidot)*sin(deltar) + psidot*cos(deltar))/v;
    }

    inline double WheelSetComponent::xi_rls(double psi, double psidot, double phidot,
            double beta, double deltal){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double b = this->bogieFrame->get_b();
        double Omega_0 = v/this->r0;
        return (-(Omega_0+beta-(psi-b/R)*phidot)*sin(deltal) + psidot*cos(deltal))/v;
    }

    inline double WheelSetComponent::xi_rrs(double psi, double psidot, double phidot,
            double beta, double deltar){
        double v = this->getExternalComponents()->get_v();
        double R = this->getExternalComponents()->get_R();
        double b = this->bogieFrame->get_b();
        double Omega_0 = v/this->r0;
        return ((Omega_0+beta-(psi-b/R)*phidot)*sin(deltar) + psidot*cos(deltar))/v;
    }

    inline double WheelSetComponent::Fx(double cp_a, double cp_b, double C11, double xi_x){
        return (-cp_a*cp_b* this->get_G() *C11*xi_x);
    }

    inline double WheelSetComponent::Fy(double cp_a, double cp_b, double C22, double C23,
            double xi_y, double xi_s){
        return (-cp_a*cp_b* this->get_G() *(C22*xi_y+sqrt(cp_a*cp_b) * C23*xi_s));
    }
}
