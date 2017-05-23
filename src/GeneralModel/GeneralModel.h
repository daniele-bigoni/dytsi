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
 * File:   GeneralModel.h
 * Author: bigo
 *
 * Created on February 12, 2011, 5:03 PM
 */

#ifndef GENERALMODEL_H
#define	GENERALMODEL_H

#include <gsl/gsl_odeiv2.h>
#include "enumDeclarations.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include "Component.h"

#include <string>
#include <vector>

namespace DYTSI_Modelling {
    
    class Connector;
    class ExternalComponents;
    class LinkSpring;

    enum ExecEnum {FUN, JAC};

    struct FunInput {
        double t;
        double* y;
        double* f;
    };

    struct JacInput {
        double t;
        double* y;
        double* dfdy;
        double* dfdt;
    };

    struct ExecInput {
        ExecEnum type;
        void* input;
        int returnCode;
    };

    class GeneralModel {
    public:
        GeneralModel();
        virtual ~GeneralModel();

        void setup(XERCES_CPP_NAMESPACE::DOMElement* el);
        bool setStartingValues(XERCES_CPP_NAMESPACE::DOMElement* el, std::vector<double>& startingValues);
//        bool setAcceleration();
        void init();
        std::string printCounters();

        // Dynamical system functions
        int computeFun( double t, const double y[], double f[] );
        int computeJac( double t, const double y[], double* dfdy, double dfdt[]);
        int computeFun( double t, double y[], double f[] );
        int computeJac( double t, double y[], double* dfdy, double dfdt[]);

        ExternalComponents* getExternalComponents(){ return externalComponents; }
        
        int get_N_DOF(){ return N_DOF; }
        void set_N_DOF(int dof){ N_DOF = dof; }

        gsl_odeiv2_system get_sys(){ return sys; }

        // Virtual methods
        virtual std::vector<Component*> getComponentList() = 0;

    private:
        bool settedUp;
        // Attributes
        int N_DOF;
        gsl_odeiv2_system sys;
        ExternalComponents* externalComponents;
        Component* rootComponent;

        unsigned long int fun_counter;
        unsigned long int jac_counter;

        bool setupStaticLoads();

    protected:
        std::vector<DYTSI_Modelling::LinkSpring*> staticLoadedSecondarySuspensionSpringList;
        std::vector<DYTSI_Modelling::LinkSpring*> staticLoadedPrimarySuspensionSpringList;

        void setExternalComponents(ExternalComponents* externalComponents){
            this->externalComponents = externalComponents;
        }

        // Virtual methods
        virtual bool setupModel(XERCES_CPP_NAMESPACE::DOMElement* el) = 0;
        virtual bool loadStartingValues( XERCES_CPP_NAMESPACE::DOMElement* el, std::vector<double>& startingValues) = 0;
        
        // Accessory functions
        int setRootComponent( Component* component );
        Component* getRootComponent(){ return this->rootComponent; }
        void setSettedUp(bool val);
        bool getSettedUp() const;
    };
}

#endif	/* GENERALMODEL_H */

