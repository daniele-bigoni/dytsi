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
 * File:   Component.cpp
 * Author: s083781
 * 
 * Created on February 11, 2011, 4:35 PM
 */

#include "Component.h"
#include "ComponentConnectorCouple.h"
#include "Connector.h"
#include "GeneralModel.h"
#include "ExternalComponents.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

#include <math.h>
#include <string.h>
#include <sstream>

namespace DYTSI_Modelling {
    Component::Component(std::string name, GeneralModel* gm,
                ExternalComponents* externalComponents,
                gsl_vector* centerOfGeometry, gsl_vector* centerOfMassWRTCenterOfGeometry,
                bool isFixed) {

    	this->upper = NULL;
    	this->lowerLeading = NULL;
    	this->lowerTrailing = NULL;

        this->id_component = this->generateID();
        this->name = name;
        this->generalModel = gm;
        this->externalComponents = externalComponents;
        this->centerOfGeometry = centerOfGeometry;
        this->centerOfMassWRTCenterOfGeometry = centerOfMassWRTCenterOfGeometry;
        this->isFixed = isFixed;

        // Compute the position of the center of mass wrt the center of the track following reference system
        this->centerOfMass = gsl_vector_alloc(3);
        gsl_vector_memcpy(this->centerOfMass, this->centerOfMassWRTCenterOfGeometry);
        gsl_blas_daxpy(1.0, this->centerOfGeometry, this->centerOfMass);

        this->connectorsForcesAndMoments = gsl_vector_alloc(6);
        vecForceAndMomentum = gsl_vector_calloc(6);
    }

    Component::~Component() {
        gsl_vector_free(this->getCenterOfMass());
    }

    int Component::generateID(){
        static int ID_COMPONENT_COUNTER = 0;
        return ID_COMPONENT_COUNTER++;
    }

    int Component::getID(){
        return id_component;
    }

    ComponentConnectorCouple* Component::getUpper() {
        return upper;
    }
    ComponentConnectorCouple* Component::getLowerLeading() {
        return lowerLeading;
    }
    ComponentConnectorCouple* Component::getLowerTrailing() {
        return lowerTrailing;
    }

    ComponentPosition Component::get_position(){
        return position;
    }

    int Component::get_startingDOF(){
        return this->startingDOF;
    }

    gsl_vector* Component::getCenterOfMass(){
        return this->centerOfMass;
    }

    /**
     * Transform the coordinates of a vector w.r.t the center of geometry to a vector
     * w.r.t the center of mass
     * @param position
     * @return
     */
    gsl_vector* Component::getPositionWRTCenterOfMass(gsl_vector* position){
        gsl_vector* out = gsl_vector_alloc(3);
        gsl_vector_memcpy(out, position);
        gsl_blas_daxpy(-1.0, this->getCenterOfMassWRTCenterOfGeometry(), out);
        return out;
    }

    gsl_vector* Component::getCenterOfMassWRTCenterOfGeometry(){
        return this->centerOfMassWRTCenterOfGeometry;
    }

    gsl_vector* Component::getCenterOfGeometry(){
        return this->centerOfGeometry;
    }
    
    GeneralModel* Component::get_generalModel(){
        return this->generalModel;
    }

    bool Component::get_isFixed(){
    	return this->isFixed;
    }

    void Component::setConnection(Connector* connector,
            Component* component, RelativeComponentPosition position){
        ComponentConnectorCouple* couple =
                new ComponentConnectorCouple(component, connector);

        switch (position){
            case UP:
                this->upper = couple;
                break;
            case LL:
                this->lowerLeading = couple;
                break;
            case LT:
                this->lowerTrailing = couple;
                break;
        }

        ///////////////////////////////////////////////////////
        // Set up interdependencies for the numerical Jacobian
        for( int i=0; i < component->get_N_DOF(); i++ ){
            this->partialJacobianIdx.push_back( component->get_startingDOF() + i );
        }
        ///////////////////////////////////////////////////////

    }

    void Component::setDOF(GeneralModel* gm){
        this->startingDOF = gm->get_N_DOF();
        gm->set_N_DOF(gm->get_N_DOF() + this->get_N_DOF());
    }

    void Component::set_name( std::string name ){ this->name = name; }

    std::string Component::get_name(){
        std::stringstream out(std::stringstream::in|std::stringstream::out);
        out << this->name << "[ID " << this->id_component << "]";
        std::string outStr = out.str();
        return outStr;
    }

    ExternalComponents* Component::getExternalComponents(){ return this->externalComponents; }
    void Component::setExternalComponents(ExternalComponents* extComp) {
        this->externalComponents = extComp;
    }

    // Compute gravitational force and moment
    gsl_vector* Component::computeGravitationalForceAndMoment( ){

        gsl_vector_view F_view = gsl_vector_subvector( vecForceAndMomentum, 0, 3);
        gsl_vector* F = &F_view.vector;

        gsl_vector_view M_view = gsl_vector_subvector( vecForceAndMomentum, 3, 3);
        gsl_vector* M = &M_view.vector;

        double sin_Phi_se = this->getExternalComponents()->get_sin_Phi_se();
        double cos_Phi_se = this->getExternalComponents()->get_cos_Phi_se();

        // Compute the force
        gsl_vector_set( F,1, - sin_Phi_se * this->get_mass() * g );
        gsl_vector_set( F,2, - cos_Phi_se * this->get_mass() * g );

        return vecForceAndMomentum;
    }

//    // Compute centrifugal force and moment
//    gsl_vector* Component::computeCentrifugalForceAndMoment( ){
//        gsl_vector* vecForceAndMomentum = gsl_vector_calloc(6);
//
//        gsl_vector_view F_view = gsl_vector_subvector( vecForceAndMomentum, 0, 3);
//        gsl_vector* F = &F_view.vector;
//
//        gsl_vector_view M_view = gsl_vector_subvector( vecForceAndMomentum, 3, 3);
//        gsl_vector* M = &M_view.vector;
//
//        double v = this->getExternalComponents()->get_v();
//        double R = this->getExternalComponents()->get_R();
//        double Phi_se = this->getExternalComponents()->get_Phi_se();
//
//        // Compute the force
//        gsl_vector_set( F,1, this->get_mass() * pow(v,2.0) / R );
//        gsl_vector_set( F,2, Phi_se * this->get_mass() * pow(v,2.0) / R );
//
//        return vecForceAndMomentum;
//    }

    int Component::fun(double t, double y[], double f[]){

    	int errCode = GSL_SUCCESS;

    	// Compute the lower leading component (if any)
		if (this->getLowerLeading() != NULL){
			errCode = this->getLowerLeading()->getComponent()->fun(t, y, f);
			if (errCode != GSL_SUCCESS){
				return errCode;
			}
		}

		// Compute the lower trailing component (if any)
		if (this->getLowerTrailing() != NULL){
			errCode = this->getLowerTrailing()->getComponent()->fun(t, y, f);
			if (errCode != GSL_SUCCESS){
				return errCode;
			}
		}

    	if (this->isFixed){

    		// Retreive the corresponding data in dfdt
			double *loc_f = &(f[this->get_startingDOF()]);
			// Derivatives w.r.t time
			for( int i = 0; i < this->get_N_DOF(); i++ ){
				loc_f[i] = .0;
			}

    		return errCode;
    	} else {
    		return this->computeFun(t, y, f);
    	}

    }

    int Component::jac(double t, double * y, double * dfdy, double * dfdt){

    	if (this->isFixed){

    		int errCode = GSL_SUCCESS;

			// Compute the lower leading component (if any)
			if (this->getLowerLeading() != NULL){
				errCode = this->getLowerLeading()->getComponent()->jac(t, y, dfdy, dfdt);
				if (errCode != GSL_SUCCESS){
					return errCode;
				}
			}

			// Compute the lower trailing component (if any)
			if (this->getLowerTrailing() != NULL){
				errCode = this->getLowerTrailing()->getComponent()->jac(t, y, dfdy, dfdt);
				if (errCode != GSL_SUCCESS){
					return errCode;
				}
			}

    		// Retreive the submatrix relative to the CarBody equations.
			int N_VAR = this->get_generalModel()->get_N_DOF();
			gsl_matrix_view dfdy_mat = gsl_matrix_view_array (dfdy,N_VAR,N_VAR);
			//gsl_matrix *MAT = &dfdy_mat.matrix;
			gsl_matrix_view sub_dfdy = gsl_matrix_submatrix(
					&dfdy_mat.matrix, this->get_startingDOF(), 0, this->get_N_DOF(), N_VAR);

			gsl_matrix_set_zero(&sub_dfdy.matrix);

	        // Retreive the corresponding data in dfdt
	        double *loc_dfdt = &(dfdt[this->get_startingDOF()]);
	        // Derivatives w.r.t time
	        for( int i = 0; i < this->get_N_DOF(); i++ ){
	            loc_dfdt[i] = .0;
	        }

			return errCode;
    	} else {
    		return this->computeJac(t, y, dfdy, dfdt);
    	}

    }

    int Component::jac_numerical(double t, const double y[], gsl_matrix *J, int var){
        int errCode = GSL_SUCCESS;

        // Copy the array y to a temporary array used for Jacobian approximation
        double* yd = new double[this->get_generalModel()->get_N_DOF()];
        memcpy(yd,y,(this->get_generalModel()->get_N_DOF())*sizeof(double));

        gsl_matrix* fy_pos = gsl_matrix_calloc(this->get_N_DOF(),1);
        gsl_matrix* fy_neg = gsl_matrix_calloc(this->get_N_DOF(),1);

//        // Set delta 1e-2 times smaller than the value or 1e-8 maximum
//        double delta = yd[var] * 1e-2;
//        if (delta < 1e-8)
//            delta = 1e-8;
        double delta = 1e-7;

        // Compute the rhs with positive disturbance
        yd[var] += delta;
        errCode = this->rhs(t, yd, fy_pos);
        if (errCode != GSL_SUCCESS)
            return errCode;
        yd[var] -= delta;

        // Compute the rhs with negative disturbance
        yd[var] -= delta;
        errCode = this->rhs(t, yd, fy_neg);
        if (errCode != GSL_SUCCESS)
            return errCode;
        yd[var] += delta;

        // Compute the central approximation of the derivative and set it up in the
        // Jacobian matrix.
        //gsl_matrix_view Jvar_view = gsl_matrix_submatrix(J, 0, var, this->get_N_DOF(), 1);
        //gsl_matrix* Jvar = &Jvar_view.matrix;
        for (int i = 0; i < this->get_N_DOF(); i++)
            gsl_matrix_set(J, i, var,
                    (gsl_matrix_get(fy_pos, i, 0)-gsl_matrix_get(fy_neg, i, 0)) /(2.0*delta));

        // Free memory
        gsl_matrix_free(fy_pos);
        gsl_matrix_free(fy_neg);
        delete []yd;

        return GSL_SUCCESS;
    }

    void Component::Execute(void* args){
        ExecInput* execInput = (ExecInput*) args;

        FunInput* funInput;
        JacInput* jacInput;
        switch (execInput->type) {
            case FUN:
                funInput = (FunInput*)execInput->input;
                execInput->returnCode = this->computeFun(
                        funInput->t, funInput->y, funInput->f);
                break;

            case JAC:
                jacInput = (JacInput*)execInput->input;
                execInput->returnCode = this->computeJac(
                        jacInput->t, jacInput->y, jacInput->dfdy, jacInput->dfdt);
                break;
        }
    }
}
