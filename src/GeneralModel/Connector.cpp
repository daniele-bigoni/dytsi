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
 * File:   Connector.cpp
 * Author: s083781
 * 
 * Created on February 11, 2011, 4:21 PM
 */

#include "Connector.h"
#include "Component.h"
#include "Link.h"

#include "../Application/PrintingHandler.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

#include <cmath>
#include <sstream>
#include <stdio.h>

namespace DYTSI_Modelling {
    Connector::Connector( std::string name,
            Component* first, Component* second,
            RelativeComponentPosition posFirst,
            RelativeComponentPosition posSecond){

        this->name = name;

        first->setConnection( this, second, posSecond );
        second->setConnection( this, first, posFirst );

        this->linkList.clear();
//        this->springList.clear();
//        this->damperList.clear();
        //this->springList = new std::vector<Spring*>();
        //this->damperList = new std::vector<Damper*>();

        if ((posFirst == UP) && ((posSecond == LL)||(posSecond == LT))){
            this->upperComponent = first;
            this->lowerComponent = second;
        } else if ((posSecond == UP) && ((posFirst == LL)||(posFirst == LT))){
            this->upperComponent = second;
            this->lowerComponent = first;
        } else {
            std::stringstream out;
            out << "Connector Error. Erroneous relative position of components." << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDERR);
            exit(-1);
        }

        this->rMain = gsl_vector_alloc(3);
        this->rAttached = gsl_vector_alloc(3);
        this->drMain = gsl_vector_alloc(3);
        this->drAttached = gsl_vector_alloc(3);
        this->totForceAndMomentum = gsl_vector_calloc(6);
    }

    Connector::~Connector() {
    }

    void Connector::addLink(Link* link){
        link->connect(this->lowerComponent, this->upperComponent);
        this->linkList.push_back(link);
    }

    Link* Connector::getLink(int idx){
        return this->linkList[idx];
    }

    /**
     * Return the vector of forces and moments (3 directional forces and 3 moments) using the
     * displacements vectors given as arguments
     * @param mainComponent
     * @param attachedComponent
     * @param dispMain
     * @param dispAttached
     * @param ddispMain
     * @param ddispAttached
     * @param transformationMatrixMain
     * @param transformationMatrixAttached
     * @param dTransformationMatrixMain
     * @param dTransformationMatrixAttached
     * @return
     */
    gsl_vector* Connector::getForcesAndMoments( RelativeComponentPosition posConnector,
            gsl_vector* dispMain, gsl_vector* dispAttached,
            gsl_vector* ddispMain, gsl_vector* ddispAttached,
            gsl_matrix* transformationMatrixMain, gsl_matrix* transformationMatrixAttached,
            gsl_matrix* dTransformationMatrixMain, gsl_matrix* dTransformationMatrixAttached,
            int& errCode){
        std::stringstream* out;

        gsl_vector_set_zero(this->totForceAndMomentum);

        // Check for the position of the mainComponent
        Component* mainComponent;
        Component* attachedComponent;
        if (posConnector == UP){
            mainComponent = this->lowerComponent;
            attachedComponent = this->upperComponent;
        } else if ((posConnector == LL) || (posConnector == LT)){
            mainComponent = this->upperComponent;
            attachedComponent = this->lowerComponent;
        } else {
            out = new std::stringstream(std::stringstream::in | std::stringstream::out);
            *out << "Error in connector \""<< this->getName() << "\": " << std::endl;
            *out << "ComputeForceAndMoment: the position passed is not valid" << std::endl;
            PrintingHandler::printOut(out, PrintingHandler::STDERR);
            delete out;
            exit(-1);
        }

        /////////
        // Compute the displaced position of the main C.M.
        gsl_vector_memcpy(rMain, dispMain);
        gsl_blas_daxpy(1.0,mainComponent->getCenterOfMass(), rMain);
        // Compute the displaced position of the attached C.M.
        gsl_vector_memcpy(rAttached, dispAttached);
        gsl_blas_daxpy(1.0,attachedComponent->getCenterOfMass(), rAttached);
        /////////

        /////////
        // Compute the speed vector of the main C.M.
        gsl_vector_memcpy(drMain, ddispMain);
        // Compute the speed vector of the attached C.M.
        gsl_vector_memcpy(drAttached, ddispAttached);
        /////////

        /////////
        // Iterate over the links
        gsl_vector* FMvec;
        for (std::vector<Link*>::iterator it = this->linkList.begin();
                it != this->linkList.end(); ++it){
            Link* link = *it;

            FMvec = link->computeForceAndMoments( posConnector, rMain, rAttached,
                    drMain, drAttached, transformationMatrixMain, transformationMatrixAttached,
                    dTransformationMatrixMain, dTransformationMatrixAttached, errCode);

            // Check for NaN values
            for( int i = 0; i < 6; i++ ){
                if ( isnan( gsl_vector_get(FMvec,i) ) ){
                    errCode = GSL_EDOM;
                    const double* mainCurrY = mainComponent->getCurrentY();
                    const double* attachedCurrY = attachedComponent->getCurrentY();
                    out = new std::stringstream(std::stringstream::in | std::stringstream::out);
                    *out << "Error in connector \""<< this->getName() << "\": " << std::endl;
                    *out << "Link \"" << link->getName() << "\" returns a NaN value." << std::endl;
                    *out << "Current displacements Main component " << mainComponent->get_name() << std::endl;
                    for (int i = 0; i < mainComponent->get_N_DOF(); i++)
                        *out << mainCurrY[i] << " ";
                    *out << std::endl;
                    *out << "Current displacements Attached component " << attachedComponent->get_name() << std::endl;
                    for (int i = 0; i < attachedComponent->get_N_DOF(); i++)
                        *out << attachedCurrY[i] << " ";
                    *out << std::endl;
                    PrintingHandler::printOut(out, PrintingHandler::STDERR);
                    delete out;
                    return totForceAndMomentum;
                }
            }

            gsl_blas_daxpy(1.0, FMvec, totForceAndMomentum);

        }
        // End iterate links

        return totForceAndMomentum;
    }
}
