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
 * File:   Connector.h
 * Author: s083781
 *
 * Created on February 11, 2011, 4:21 PM
 */

#ifndef CONNECTOR_H
#define	CONNECTOR_H

#include "enumDeclarations.h"

#include <memory>
#include <vector>
#include <string>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

namespace DYTSI_Modelling {
    class Component;
    class Link;

    class Connector {
    public:
        Connector( std::string name, Component* first,
                Component* second,
                RelativeComponentPosition posFirst,
                RelativeComponentPosition posSecond);
        virtual ~Connector();

        void addLink(Link* link);

        Link* getLink(int idx);

        int getLinkListSize(){ return this->linkList.size(); }

        std::string getName(){ return this->name; }

        gsl_vector* getForcesAndMoments( RelativeComponentPosition posConnector,
            gsl_vector* dispMain, gsl_vector* dispAttached,
            gsl_vector* ddispMain, gsl_vector* ddispAttached,
            gsl_matrix* transformationMatrixMain, gsl_matrix* transformationMatrixAttached,
            gsl_matrix* dTransformationMatrixMain, gsl_matrix* dTransformationMatrixAttached,
            int& errCode);

    private:
        std::string name;
        
        Component* upperComponent;
        Component* lowerComponent;

        gsl_vector* rMain;
        gsl_vector* rAttached;
        gsl_vector* drMain;
        gsl_vector* drAttached;
        gsl_vector* totForceAndMomentum;

    protected:
        std::vector<Link*> linkList;
        
    };
}

#endif	/* CONNECTOR_H */

