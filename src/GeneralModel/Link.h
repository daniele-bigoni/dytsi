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
 * File:   Link.h
 * Author: bigo
 *
 * Created on March 1, 2011, 9:04 PM
 */

#ifndef LINK_H
#define	LINK_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "enumDeclarations.h"

#include <string>

namespace DYTSI_Modelling{
    class Component;

    class Link {
    public:
        Link(std::string name, gsl_vector* connectionPointUp, gsl_vector* connectionPointDown);
        virtual ~Link();

        std::string getName();
        int getID();
        int generateID();

        gsl_vector* getConnectionPointUp() { return this->connectionPointUp; }
        gsl_vector* getConnectionPointDown() { return this->connectionPointDown; }

        gsl_vector* getConnectionPointUpWRTCenterOfGeometry() { return this->connectionPointUpWRTCenterOfGeometry; }
        gsl_vector* getConnectionPointDownWRTCenterOfGeometry() { return this->connectionPointDownWRTCenterOfGeometry; }

        Component* getUpperComponent() { return this->upperComponent; }
        Component* getLowerComponent() { return this->lowerComponent; }

        void connect(Component* lowerComponent, Component* upperComponent);
        void setVecLowerToUpperAtRest(gsl_vector* );
        gsl_vector* getVecLowerToUpperAtRest();
        void correctForStaticLoads(gsl_vector* disp);

        virtual gsl_vector* computeForceAndMoments(
                RelativeComponentPosition posConnector,
                gsl_vector* rMain, gsl_vector* rAttached,
                gsl_vector* ddispMain, gsl_vector* ddispAttached,
                gsl_matrix* transformationMatrixMain, gsl_matrix* transformationMatrixAttached,
                gsl_matrix* dTransformationMatrixMain, gsl_matrix* dTransformationMatrixAttached,
                int& errCode) = 0;
    private:
        int id_link;
        std::string name;
        
        Component* upperComponent;
        Component* lowerComponent;
        gsl_vector* connectionPointUp;
        gsl_vector* connectionPointDown;
        gsl_vector* connectionPointUpWRTCenterOfGeometry;
        gsl_vector* connectionPointDownWRTCenterOfGeometry;

        gsl_vector* vecLowerToUpperAtRest;

    protected:
        void computeTorques( gsl_vector* point, gsl_vector* F, gsl_vector* M );
    };
}

#endif	/* LINK_H */

