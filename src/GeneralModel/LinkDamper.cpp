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
 * File:   Damper.cpp
 * Author: s083781
 * 
 * Created on February 24, 2011, 12:39 PM
 */

#include "LinkDamper.h"

#include <gsl/gsl_blas.h>
#include <gsl/gsl_math.h>
#include <sstream>
#include "../Application/PrintingHandler.h"

namespace DYTSI_Modelling{
    LinkDamper::LinkDamper(std::string name, gsl_vector* connectionPointUp, gsl_vector* connectionPointDown)
    : Link(name, connectionPointUp, connectionPointDown){
        this->rMainDamper = gsl_vector_alloc(3);
        this->rAttachedDamper = gsl_vector_alloc(3);
        this->length = gsl_vector_alloc(3);
        this->drMainDamper = gsl_vector_alloc(3);
        this->drAttachedDamper = gsl_vector_alloc(3);
        this->v = gsl_vector_alloc(3);
        this->F = gsl_vector_alloc(3);
        this->F_rel = gsl_vector_alloc(3);
        this->M = gsl_vector_alloc(3);
        this->Fb = gsl_vector_alloc(3);
        this->vecForceAndMomentum = gsl_vector_alloc(6);
    }

    LinkDamper::~LinkDamper() {
    }

    gsl_vector* LinkDamper::computeForceAndMoments(
                RelativeComponentPosition posConnector,
                gsl_vector* rMain, gsl_vector* rAttached,
                gsl_vector* drMain, gsl_vector* drAttached,
                gsl_matrix* transformationMatrixMain, gsl_matrix* transformationMatrixAttached,
                gsl_matrix* dTransformationMatrixMain, gsl_matrix* dTransformationMatrixAttached,
                int& errCode){
        
        // Set the main and attached components/connection points
        Component* mainComponent;
        Component* attachedComponent;
        gsl_vector* connectionPointMain;
        gsl_vector* connectionPointAttached;
        if (posConnector == UP){
            mainComponent = this->getLowerComponent();
            connectionPointMain = this->getConnectionPointDown();
            attachedComponent = this->getUpperComponent();
            connectionPointAttached = this->getConnectionPointUp();
        } else if ((posConnector == LL) || (posConnector == LT)){
            mainComponent = this->getUpperComponent();
            connectionPointMain = this->getConnectionPointUp();
            attachedComponent = this->getLowerComponent();
            connectionPointAttached = this->getConnectionPointDown();
        } else {
            std::stringstream out;
            out << "Damper Error: computeForceAndMoment: the position passed is not valid" << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDERR);
            exit(-1);
        }

        // Compute the main attach position
        // rMainSpring = transformationMatrixMain*attachPointSpring + rMain
        // int gsl_blas_dgemv (CBLAS_TRANSPOSE_t TransA, double alpha, const gsl_matrix * A, const gsl_vector * x, double beta, gsl_vector * y)
        // gsl_blas_dgemv: y = \alpha op(A) x + \beta y
        gsl_vector_memcpy(rMainDamper, rMain);
        gsl_blas_dgemv(CblasNoTrans, 1.0, transformationMatrixMain, connectionPointMain,
                        1.0, rMainDamper);
        // Compute the attached attach position
        gsl_vector_memcpy( rAttachedDamper, rAttached );
        gsl_blas_dgemv(CblasNoTrans, 1.0, transformationMatrixAttached, connectionPointAttached,
                        1.0, rAttachedDamper);
        // Find the length vector Main-Attached
        // length = rAttachedSpring - rMainSpring
        gsl_vector_memcpy(length, rAttachedDamper);
        gsl_blas_daxpy(-1.0, rMainDamper, length);

        // Compute the main speed of the attack point
        // drMainDamper = dtransformationMatrixMain*attachPointSpring + drMain
        // int gsl_blas_dgemv (CBLAS_TRANSPOSE_t TransA, double alpha, const gsl_matrix * A, const gsl_vector * x, double beta, gsl_vector * y)
        // gsl_blas_dgemv: y = \alpha op(A) x + \beta y
        gsl_vector_memcpy(drMainDamper, drMain);
        gsl_blas_dgemv(CblasNoTrans, 1.0, dTransformationMatrixMain, connectionPointMain,
                        1.0, drMainDamper);
        // Compute the speed of the attached attack point
        gsl_vector_memcpy( drAttachedDamper, drAttached );
        gsl_blas_dgemv(CblasNoTrans, 1.0, dTransformationMatrixAttached, connectionPointAttached,
                        1.0, drAttachedDamper);
        // Find the velocity vector
        // v = drAttachedDamper - drMainDamper
        gsl_vector_memcpy(v, drAttachedDamper);
        gsl_blas_daxpy(-1.0, drMainDamper, v);

        // Compute the force
        errCode = this->force(posConnector, v, length, F);
        if (errCode != GSL_SUCCESS){
            std::stringstream out;
            out << "Damper Error: " << this->getName() << ": Errors occurred during the calculation of the forces." << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDERR);
            gsl_vector* vecForceAndMomentum = gsl_vector_alloc(6);
            gsl_vector_set_all(vecForceAndMomentum,GSL_NAN);
            return vecForceAndMomentum;
        }

        // Rotate the Force in the relative reference frame
        // F_rel = Tmain * F
        gsl_vector_set_zero( F_rel );
        gsl_blas_dgemv(CblasTrans, 1.0, transformationMatrixMain, F,
                        0.0, F_rel);

        /////// TORQUES
        // Compute the momentum vector
        this->computeTorques( connectionPointMain, F_rel, M);
        /////// END TORQUES

        // Pack in an unique vector
        gsl_vector_set(vecForceAndMomentum, 0, gsl_vector_get(F, 0));
        gsl_vector_set(vecForceAndMomentum, 1, gsl_vector_get(F, 1));
        gsl_vector_set(vecForceAndMomentum, 2, gsl_vector_get(F, 2));
        gsl_vector_set(vecForceAndMomentum, 3, gsl_vector_get(M, 0));
        gsl_vector_set(vecForceAndMomentum, 4, gsl_vector_get(M, 1));
        gsl_vector_set(vecForceAndMomentum, 5, gsl_vector_get(M, 2));

        return vecForceAndMomentum;
    }
}
