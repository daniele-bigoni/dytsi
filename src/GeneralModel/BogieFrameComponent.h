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
 * File:   BogieFrameComponent.h
 * Author: bigo
 *
 * Created on February 12, 2011, 4:50 PM
 */

#ifndef BOGIEFRAMECOMPONENT_H
#define	BOGIEFRAMECOMPONENT_H

#include "Component.h"
#include "enumDeclarations.h"
#include <stdio.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>

namespace DYTSI_Modelling {
    class ExternalComponents;
    class GeneralModel;
    class WheelSetBogieFrameConnector;
    class BogieFrameCarBodyConnector;
    class CarBodyComponent;
    class WheelSetComponent;

    class BogieFrameComponent : public Component{
    public:
        BogieFrameComponent(ComponentPosition position,
                gsl_vector* centerOfGeometry,
                gsl_vector* centerOfMass,
                double mass,
                double Ix,
                double Iy,
                double Iz,
                double b,
                ExternalComponents* externalComponents,
                std::string name,
                GeneralModel* gm,
                bool isFixed = false);
        virtual ~BogieFrameComponent();

        static const int N_DOF = 10;

//        static const int X = 0;
//        static const int XDOT = 1;
        static const int Y = 0;
        static const int YDOT = 1;
        static const int Z = 2;
        static const int ZDOT = 3;
        static const int PHI = 4;
        static const int PHIDOT = 5;
        static const int CHI = 6;
        static const int CHIDOT = 7;
        static const int PSI = 8;
        static const int PSIDOT = 9;
        static const std::string* DOF_Names[N_DOF];

        // Implementation of virtual Component classes
        int computeFun( double t, double y[], double f[] );
        int computeJac( double t, double y[], double* dfdy, double dfdt[]);
        void updateTransformationMatrix( gsl_matrix* mat, double y[]);
        void updateDTransformationMatrix( gsl_matrix* mat, double y[]);
        void updateDisplacementVector( gsl_vector* vec, double y[] );
        void updateDDisplacementVector( gsl_vector* vec, double y[] );
        void updateTransformationMatrix( gsl_matrix* mat, const double y[]);
        void updateDTransformationMatrix( gsl_matrix* mat, const double y[]);
        void updateDisplacementVector( gsl_vector* vec, const double y[] );
        void updateDDisplacementVector( gsl_vector* vec, const double y[] );

        int get_N_DOF() { return N_DOF; }

        bool loadStartingValues(XERCES_CPP_NAMESPACE::DOMElement* el, std::vector<double>& startingValues);
        std::vector<std::string> getHeader();
        void getStatus(double t, double* y, double* status);
        const double* getCurrentY();
        void init();
        std::string printCounters();

        double get_mass(){ return mass; }
        double get_b() { return b; }
        double get_Ix(){ return Ix; }
        double get_Iy(){ return Iy; }
        double get_Iz(){ return Iz; }

    private:

        ComponentPosition position;
        double mass;
        double Ix;
        double Iy;
        double Iz;
        double b;
        ExternalComponents* externalComponents;

        bool recalc;

        // Shortcuts
        WheelSetBogieFrameConnector* llConnector;
        WheelSetBogieFrameConnector* ltConnector;
        BogieFrameCarBodyConnector* upConnector;
        WheelSetComponent* llComponent;
        WheelSetComponent* ltComponent;
        CarBodyComponent* upComponent;

        const double* locY;
        const double* llY;
        const double* ltY;
        const double* upY;
        double* locF;

        // Transformation matrices
        gsl_matrix* locTransformationMatrix;
        gsl_matrix* locDTransformationMatrix;
        gsl_matrix* upTransformationMatrix;
        gsl_matrix* upDTransformationMatrix;
        gsl_matrix* llTransformationMatrix;
        gsl_matrix* llDTransformationMatrix;
        gsl_matrix* ltTransformationMatrix;
        gsl_matrix* ltDTransformationMatrix;
        // Displacement vectors
        gsl_vector* locDisp;
        gsl_vector* locDDisp;
        gsl_vector* upDisp;
        gsl_vector* upDDisp;
        gsl_vector* llDisp;
        gsl_vector* llDDisp;
        gsl_vector* ltDisp;
        gsl_vector* ltDDisp;

        gsl_vector* vecForceAndMoment;


    protected:
        int computeConnectorsForcesAndMoments();
        gsl_vector* computeCentrifugalForceAndMoment();
        int rhs(double t, const double y[], gsl_matrix* outRHS);
    };
}

#endif	/* BOGIEFRAMECOMPONENT_H */

