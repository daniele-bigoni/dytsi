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
 * File:   WheelSetComponent.h
 * Author: bigo
 *
 * Created on February 12, 2011, 4:01 PM
 */

#ifndef WHEELSETCOMPONENT_H
#define	WHEELSETCOMPONENT_H

#include "Component.h"
#include "enumDeclarations.h"

#include <stdio.h>
#include <vector>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>

namespace DYTSI_Modelling {

    class ExternalComponents;
    class RSGEOTable;
    class BogieFrameComponent;
    class WheelSetBogieFrameConnector;
    class GeneralModel;

    class WheelSetComponent : public Component{
    public:
        WheelSetComponent( ComponentPosition position,
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
                bool isFixed = false);
        virtual ~WheelSetComponent();

        static const int N_DOF = 9;

//        static const int X = 0;
//        static const int XDOT = 1;
        static const int Y = 0;
        static const int YDOT = 1;
        static const int Z = 2;
        static const int ZDOT = 3;
        static const int PHI = 4;
        static const int PHIDOT = 5;
        static const int BETA = 6;
        static const int PSI = 7;
        static const int PSIDOT = 8;
        static const std::string* DOF_Names[N_DOF];
        static const std::string* C_Forces_Names[2][3];
        static const std::string* N_Forces_Names[2][2];

        // Implementation of virtual Component classes
        int computeFun( double t, double y[], double f[] );
        int computeJac( double t, double y[], double* dfdy, double dfdt[]);
        void updateTransformationMatrix( gsl_matrix* mat, const double y[]);
        void updateDTransformationMatrix( gsl_matrix* mat, const double y[]);
        void updateDisplacementVector( gsl_vector* vec, const double y[] );
        void updateDDisplacementVector( gsl_vector* vec, const double y[] );
        void updateTransformationMatrix( gsl_matrix* mat, double y[]);
        void updateDTransformationMatrix( gsl_matrix* mat, double y[]);
        void updateDisplacementVector( gsl_vector* vec, double y[] );
        void updateDDisplacementVector( gsl_vector* vec, double y[] );
        
        int get_N_DOF() { return N_DOF; }

        bool loadStartingValues(XERCES_CPP_NAMESPACE::DOMElement* el, std::vector<double>& startingValues);
        std::vector<std::string> getHeader();
        void getStatus(double t, double* y, double* status);
        const double* getCurrentY();
        void init();
        std::string printCounters();

        // Extension of the connection function
        void setConnection(Connector* connector,
                Component* component,
                RelativeComponentPosition position);

        double get_mass(){ return mass; }
        double get_Ix(){ return Ix; }
        double get_Iy(){ return Iy; }
        double get_Iz(){ return Iz; }
        double get_a(){ return a; }
        double get_mx(){ return mx; }
        double get_mu(){ return mu; }
        double get_G(){ return G; }

    private:
        typedef Component super;

        enum WheelSide {LEFT, RIGHT};
        enum ContactForcesDirections {FX, FY, FZ};
        enum NormalForcesDirections {NY, NZ};
        enum TorquesDirections {TPHI, TCHI, TPSI};

        ComponentPosition position;
        double mass;
        double Ix;
        double Iy;
        double Iz;
        double mx;  // Mass of the load on the particular frame
        double mu;  // Friction coeff
        double G;   // Shear modulus
        double r0;  // Rolling radio at the central position
        double a;

        double MAX_DISPL;
        std::vector<RSGEOTable*> rsgeo_table_list;
        bool recalc;
        double **rsgeodata; // [2][N_COLS_RSGEO]
        double **c_forces;   // 2x3
        double **n_forces;   // 2x2
        double **torques;    // 2x3

        // Shortcuts
        WheelSetBogieFrameConnector *connector;
        BogieFrameComponent *bogieFrame;

        const double* locY;
        const double* upY;
        double* locF;

        gsl_matrix* locTransformationMatrix;
        gsl_matrix* locDTransformationMatrix;
        gsl_matrix* upTransformationMatrix;
        gsl_matrix* upDTransformationMatrix;
        gsl_vector* locDisp;
        gsl_vector* locDDisp;
        gsl_vector* upDisp;
        gsl_vector* upDDisp;

        // Centrifugal forces and moments
        gsl_vector* vecForceAndMoment;

        // Private methods
        int find_contact_forces(double t);
        int find_rsgeodata(double t, RSGEOTable* rsg);
        int SHE(double t, int wheel, double *out); 
        void dynamic_update( double t, int wheel );
        void dynamic_aux( double radius, double aw, double at, double zt, double delta, double y_lat,
            double z_ver, double phi, int wheel);

        // Inline functions
        double xi_flx(double psi, double psidot, double phidot, double ydot, double beta, double rl, double al); 
        double xi_frx(double psi, double psidot, double phidot, double ydot, double beta, double rr, double ar); 
        double xi_fly(double psi, double phi, double phidot, double ydot, double zdot, double rl, double al, double deltal); 
        double xi_fry(double psi, double phi, double phidot, double ydot, double zdot, double rr, double ar, double deltar); 
        double xi_rlx(double psi, double psidot, double phidot, double ydot, double beta, double rl, double al); 
        double xi_rrx(double psi, double psidot, double phidot, double ydot, double beta, double rr, double ar); 
        double xi_rly(double psi, double phi, double phidot, double ydot, double zdot, double rl, double al, double deltal); 
        double xi_rry(double psi, double phi, double phidot, double ydot, double zdot, double rr, double ar, double deltar); 
        double xi_fls(double psi, double psidot, double phidot, double beta, double deltal); 
        double xi_frs(double psi, double psidot, double phidot, double beta, double deltar); 
        double xi_rls(double psi, double psidot, double phidot, double beta, double deltal); 
        double xi_rrs(double psi, double psidot, double phidot, double beta, double deltar); 
        double Fx(double cp_a, double cp_b, double C11, double xi_x); 
        double Fy(double cp_a, double cp_b, double C22, double C23, double xi_y, double xi_s); 

    protected:
        int computeConnectorsForcesAndMoments();
        gsl_vector* computeCentrifugalForceAndMoment();
        int rhs(double t, const double y[], gsl_matrix* rightHandSide); // TODO finish BETA
    };
}

#endif	/* WHEELSETCOMPONENT_H */

