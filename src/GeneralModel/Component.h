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
 * File:   Component.h
 * Author: s083781
 *
 * Created on February 11, 2011, 4:35 PM
 */

#ifndef COMPONENT_H
#define	COMPONENT_H

#include "enumDeclarations.h"

#include "Thread.h"

#include <memory>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>

namespace DYTSI_Modelling {
    class ExternalComponents;
    class Connector;
    class ComponentConnectorCouple;
    class GeneralModel;

    const double g = 9.81;

    class Component : public Thread {
    public:

        Component(std::string name, GeneralModel* gm,
                ExternalComponents* externalComponents,
                gsl_vector* centerOfGeometry, gsl_vector* centerOfMass,
                bool isFixed);
        virtual ~Component();

        ComponentConnectorCouple* getUpper();
        ComponentConnectorCouple* getLowerLeading();
        ComponentConnectorCouple* getLowerTrailing();

        ComponentPosition get_position();

        int get_startingDOF();

        void set_name( std::string name );
        std::string get_name();
        int getID();
        int generateID();

        gsl_vector* getCenterOfMass();
        gsl_vector* getPositionWRTCenterOfMass(gsl_vector* position);
        gsl_vector* getCenterOfMassWRTCenterOfGeometry();
        gsl_vector* getCenterOfGeometry();

        bool get_isFixed();

        ExternalComponents* getExternalComponents();
        void setExternalComponents( ExternalComponents* extComp );

        // Add a connection with a neighbor level
        void setConnection(Connector* connector,
                Component* component,
                RelativeComponentPosition position);
        void setDOF( GeneralModel* gm );

        // Compute gravitational force and moment
        gsl_vector* computeGravitationalForceAndMoment( );

        // Function and Jacobian public handler (these take care of the case in which
        // the component is fixed)
        int fun(double t, double y[], double f[]);
        int jac(double t, double y[], double* dfdy, double dfdt[]);

        // Virtual methods
        virtual void updateTransformationMatrix( gsl_matrix* mat, double y[] ) = 0;
        virtual void updateDTransformationMatrix( gsl_matrix* mat, double y[] ) = 0;
        virtual void updateDisplacementVector( gsl_vector* vec, double y[] ) = 0;
        virtual void updateDDisplacementVector( gsl_vector* vec, double y[] ) = 0;
        virtual void updateTransformationMatrix( gsl_matrix* mat, const double y[] ) = 0;
        virtual void updateDTransformationMatrix( gsl_matrix* mat, const double y[] ) = 0;
        virtual void updateDisplacementVector( gsl_vector* vec, const double y[] ) = 0;
        virtual void updateDDisplacementVector( gsl_vector* vec, const double y[] ) = 0;

        virtual int get_N_DOF() = 0;
        virtual double get_mass() = 0;

        virtual bool loadStartingValues(XERCES_CPP_NAMESPACE::DOMElement* el, std::vector<double>& startingValues) = 0;
        virtual std::vector<std::string> getHeader() = 0;
        virtual void getStatus(double t, double* y, double* status) = 0;
        virtual const double* getCurrentY() = 0;
        virtual void init() = 0;
        virtual std::string printCounters() = 0;
        
    private:
        int id_component;

        ComponentPosition position;

        ComponentConnectorCouple* upper;
        ComponentConnectorCouple* lowerLeading;
        ComponentConnectorCouple* lowerTrailing;

        int startingDOF;

        bool isFixed;

        std::string name;
        GeneralModel *generalModel;

        gsl_vector* centerOfMass;
        gsl_vector* centerOfMassWRTCenterOfGeometry;
        gsl_vector* centerOfGeometry;

        gsl_vector* vecForceAndMomentum;

        ExternalComponents* externalComponents;

    protected:
        GeneralModel* get_generalModel();
        unsigned long int rhs_counter;
        
        // Indices of the variable for partial derivatives in the Jacobian
        std::vector<int> partialJacobianIdx;

        // Vector that contains the sum of all the forces and moments caused by a connector
        // at a certain time with certain displacements
        gsl_vector* connectorsForcesAndMoments;

        int jac_numerical(double t, const double y[], gsl_matrix *J, int var);

        // Virtual Methods
        virtual int computeFun( double t, double y[], double f[] ) = 0;
        virtual int computeJac( double t, double y[], double* dfdy, double dfdt[] ) = 0;
        virtual gsl_vector* computeCentrifugalForceAndMoment( ) = 0;
        virtual int computeConnectorsForcesAndMoments() = 0;
        virtual int rhs(double t, const double y[], gsl_matrix* outRHS) = 0;

        // Implementation of virtual Thread methods
        void Execute(void*);

    };

}

#endif	/* COMPONENT_H */

