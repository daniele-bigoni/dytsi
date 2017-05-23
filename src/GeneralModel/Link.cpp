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
 * File:   Link.cpp
 * Author: bigo
 * 
 * Created on March 1, 2011, 9:04 PM
 */

#include "Link.h"

#include "Component.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

#include <sstream>

namespace DYTSI_Modelling{
    Link::Link(std::string name, gsl_vector* connectionPointUpWRTCenterOfGeometry, gsl_vector* connectionPointDownWRTCenterOfGeometry) {
        this->id_link = this->generateID();
        
        this->name = name;
        this->connectionPointDownWRTCenterOfGeometry = connectionPointDownWRTCenterOfGeometry;
        this->connectionPointUpWRTCenterOfGeometry = connectionPointUpWRTCenterOfGeometry;
    }

    Link::~Link() {
        gsl_vector_free(this->getConnectionPointDown());
        gsl_vector_free(this->getConnectionPointUp());
    }

    int Link::getID(){
        return id_link;
    }

    void Link::connect(Component* lowerComponent, Component* upperComponent){
        this->lowerComponent = lowerComponent;
        this->upperComponent = upperComponent;

        // Find the connection point positions w.r.t. the centers of mass
        this->connectionPointUp = this->upperComponent->getPositionWRTCenterOfMass(
                this->connectionPointUpWRTCenterOfGeometry);
        this->connectionPointDown = this->lowerComponent->getPositionWRTCenterOfMass(
                this->connectionPointDownWRTCenterOfGeometry);

        // Set up the rest vector
        gsl_vector* yUp = gsl_vector_alloc(3);
        gsl_vector* yDown = gsl_vector_alloc(3);

        // Find position of the upper coonection point in the inertial frame
        gsl_vector_memcpy( yUp, this->getConnectionPointUp() );
        gsl_blas_daxpy(1.0, this->upperComponent->getCenterOfMass(), yUp);

        // Find position of the lower coonection point in the inertial frame
        gsl_vector_memcpy( yDown, this->getConnectionPointDown() );
        gsl_blas_daxpy(1.0, this->lowerComponent->getCenterOfMass(), yDown);

        // Take the difference yUp =yUp - yDown
        gsl_blas_daxpy(-1.0, yDown, yUp);

        this->vecLowerToUpperAtRest = yUp;
    }

    void Link::setVecLowerToUpperAtRest(gsl_vector* vecLowerToUpperAtRest){
        this->vecLowerToUpperAtRest = vecLowerToUpperAtRest;
    }

    gsl_vector* Link::getVecLowerToUpperAtRest(){
        return this->vecLowerToUpperAtRest;
    }

    void Link::correctForStaticLoads(gsl_vector* disp){
        // add the two vectors
        gsl_blas_daxpy(-1.0, disp, this->vecLowerToUpperAtRest);
    }

    void Link::computeTorques( gsl_vector* point, gsl_vector* F, gsl_vector* M ){
        double Mx = gsl_vector_get(F,2) * gsl_vector_get(point,1) - gsl_vector_get(F,1) * gsl_vector_get(point,2);
        double My = gsl_vector_get(F,0) * gsl_vector_get(point,2) - gsl_vector_get(F,2) * gsl_vector_get(point,0);
        double Mz = gsl_vector_get(F,1) * gsl_vector_get(point,0) - gsl_vector_get(F,0) * gsl_vector_get(point,1);
        gsl_vector_set(M, 0, Mx );
        gsl_vector_set(M, 1, My );
        gsl_vector_set(M, 2, Mz );
    }

    std::string Link::getName(){ 
        std::stringstream out(std::stringstream::in|std::stringstream::out);
        out << this->name << "[ID " << this->id_link << "]";
        std::string outStr = out.str();
        return outStr;
    }

    int Link::generateID(){
        static int ID_LINK_COUNTER = 0;
        return ID_LINK_COUNTER++;
    }
}
