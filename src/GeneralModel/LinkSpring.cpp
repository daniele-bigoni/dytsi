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
 * File:   Springa.cpp
 * Author: s083781
 * 
 * Created on February 24, 2011, 12:31 PM
 */

#include "LinkSpring.h"

#include "Link.h"
#include "../Application/PrintingHandler.h"

#include "gsl/gsl_vector.h"
#include "Function.h"
#include <gsl/gsl_blas.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>

#include <sstream>

namespace DYTSI_Modelling{
    struct StaticLoadStruct{
        double Fg;
        LinkSpring* spring;
    };

    double staticLoadFunction(double displacement, void* params){
        int errCode = GSL_SUCCESS;

        StaticLoadStruct *p = (StaticLoadStruct*) params;

        double Fs;
        errCode = p->spring->getStiffnessFunctionZ()->func(displacement, Fs);
        if (errCode != GSL_SUCCESS){
            std::stringstream out;
            out << "Spring Error: The static load is out of the range of work of the spring." << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDERR);
            exit(-1);
        }

        double out = (Fs - p->Fg);
        return out;
    }

    LinkSpring::LinkSpring(std::string name, gsl_vector* connectionPointUp, gsl_vector* connectionPointDown)
    : Link(name, connectionPointUp, connectionPointDown){
        this->rMainSpring = gsl_vector_alloc(3);
        this->rAttachedSpring = gsl_vector_alloc(3);
        this->length = gsl_vector_alloc(3);
        this->F_rel = gsl_vector_alloc(3);
        this->M = gsl_vector_alloc(3);
        this->F = gsl_vector_alloc(3);
        this->Fb = gsl_vector_alloc(3);
        this->vecForceAndMomentum = gsl_vector_alloc(6);
    }

    LinkSpring::~LinkSpring() {
    }

    gsl_vector* LinkSpring::computeForceAndMoments(
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
            out << "Spring Error: computeForceAndMoment: the position passed is not valid" << std::endl;
            PrintingHandler::printOut(&out, PrintingHandler::STDERR);
            exit(-1);
        }

        // Compute the main attach position
        // rMainSpring = transformationMatrixMain*attachPointSpring + rMain
        // int gsl_blas_dgemv (CBLAS_TRANSPOSE_t TransA, double alpha, const gsl_matrix * A, const gsl_vector * x, double beta, gsl_vector * y)
        // gsl_blas_dgemv: y = \alpha op(A) x + \beta y
        gsl_vector_memcpy(rMainSpring, rMain);
        gsl_blas_dgemv(CblasNoTrans, 1.0, transformationMatrixMain, connectionPointMain,
                        1.0, rMainSpring);
        // Compute the attached attach position
        gsl_vector_memcpy( rAttachedSpring, rAttached );
        gsl_blas_dgemv(CblasNoTrans, 1.0, transformationMatrixAttached, connectionPointAttached,
                        1.0, rAttachedSpring);

        // Find the length vector
        // length = rAttachedSpring - rMainSpring
        gsl_vector_memcpy(length, rAttachedSpring);
        gsl_blas_daxpy(-1.0, rMainSpring, length);

        // Compute the force using the function proper of the Spring implementation
        errCode = this->force(posConnector, length, F);
        if (errCode != GSL_SUCCESS){
            std::stringstream out;
            out << "Spring Error: " << this->getName() << ": Errors occurred during the calculation of the forces." << std::endl;
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

        // Pack in a unique vector
        gsl_vector_set(vecForceAndMomentum, 0, gsl_vector_get(F, 0));
        gsl_vector_set(vecForceAndMomentum, 1, gsl_vector_get(F, 1));
        gsl_vector_set(vecForceAndMomentum, 2, gsl_vector_get(F, 2));
        gsl_vector_set(vecForceAndMomentum, 3, gsl_vector_get(M, 0));
        gsl_vector_set(vecForceAndMomentum, 4, gsl_vector_get(M, 1));
        gsl_vector_set(vecForceAndMomentum, 5, gsl_vector_get(M, 2));

        return vecForceAndMomentum;

    }

    bool LinkSpring::setStaticLoad(double Fg){
        // Compute the displacement due to the static load
        // Do this using a root finding on the Force-Displacement characteristic function
        int status;
        int iter = 0, max_iter = 100;
        const gsl_root_fsolver_type *T;
        gsl_root_fsolver *s;
        double r = 0;
        double x_lo = -0.4, x_hi = 0.0;
        struct StaticLoadStruct params = { Fg, this };

        gsl_function F;
        F.function = &staticLoadFunction;
        F.params = &params;

        T = gsl_root_fsolver_brent;
        s = gsl_root_fsolver_alloc (T);
        gsl_root_fsolver_set (s, &F, x_lo, x_hi);

        do
         {
           iter++;
           status = gsl_root_fsolver_iterate (s);
           r = gsl_root_fsolver_root (s);
           x_lo = gsl_root_fsolver_x_lower (s);
           x_hi = gsl_root_fsolver_x_upper (s);
           status = gsl_root_test_interval (x_lo, x_hi,
                                            0, 1e-6);
         }
        while (status == GSL_CONTINUE && iter < max_iter);

        if ((iter == max_iter)||(status != GSL_SUCCESS)){
           std::stringstream out;
           out << "Error: " << this->getName() << ": the static load computation didn't converge" << std::endl;
           PrintingHandler::printOut(&out, PrintingHandler::STDERR);
           return true;
        } else {
            // Set the displacment as additional length in the z direction of the length at rest
            gsl_vector* disp = gsl_vector_calloc(3);
            gsl_vector_set(disp,2,r);
            this->correctForStaticLoads(disp);
        }

        return false;

    }

}

