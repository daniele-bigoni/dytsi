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
 * File:   Solution.cpp
 * Author: s083781
 * 
 * Created on March 16, 2011, 2:14 PM
 */

#include "Solution.h"

#include "../GeneralModel/ExternalComponents.h"
#include "../Application/Output/OutputHandler.h"

namespace DYTSI_Solver{
    Solution::Solution(DYTSI_Modelling::GeneralModel* gm, DYTSI_Output::OutputHandler* outputHandler) {
        this->gm = gm;
        this->outputHandler = outputHandler;

        this->reset();
    }

    Solution::~Solution() {
        delete &header;
        delete &solList;
        delete &statusList;
        delete &componentsStartingPosition;
    }

    void Solution::reset(){
        this->header.clear();
        this->solList.clear();
        this->statusList.clear();
        this->jacList.clear();
        this->componentList.clear();

        this->setComponentList();
        this->setHeader();
        this->N_VARS = this->getHeader().size();

        this->outputHandler->reset();
        this->outputHandler->writeSolutionHeader(this->getHeader());
    }

    std::vector<std::string> Solution::getHeader(){
        return this->header;
    }

    std::vector<double*> Solution::getSolutionList(){
        return this->solList;
    }

    std::vector<double*> Solution::getStatusList(){
        return this->statusList;
    }

    int Solution::getN_VARS() {
        return this->N_VARS;
    }

    void Solution::addEntry(int index, double t, double* y, double h){
        double* e = new double[this->getN_VARS()];

        // Set Index
        e[this->indexPosition] = index;

        // Set Time
        e[this->tPosition] = t;

        // Set Time Step
        e[this->tStepPosition] = h;
        
        // Set External Components Status
        DYTSI_Modelling::ExternalComponents* extComp = this->gm->getExternalComponents();
        extComp->getStatus( &e[this->extComponentStartingPosition] );

        // Set Components Status
        for (unsigned int i = 0; i < this->componentList.size(); i++){
            DYTSI_Modelling::Component* comp = this->componentList[i];
            int compStartPosition = this->componentsStartingPosition[i];
            comp->getStatus( t, y, &e[compStartPosition] );
        }

        this->solList.push_back(e);

        // Write to file
        this->outputHandler->writeSolutionEntry(e, this->N_VARS);

        // Store the status y
        double* status = new double[this->gm->get_N_DOF()];
        memcpy(status, y, this->gm->get_N_DOF() * sizeof(double));
        this->statusList.push_back( status );
    }

    void Solution::addJacobian(double* dfdy){
        int N_DOF = this->gm->get_N_DOF();

        // Copy the actual jacobian
        double* jac = new double[N_DOF * N_DOF];
        memcpy(jac, dfdy, N_DOF * N_DOF * sizeof(double));

        // Add to the list of jacobians
        this->jacList.push_back(jac);

        // Write to file
        this->outputHandler->writeJacobian(jac, N_DOF, this->jacList.size());
    }

    void Solution::setComponentList(){
        this->componentList = gm->getComponentList();
    }

    void Solution::setHeader(){
        std::string indexHeader("Index");
        this->indexPosition = this->header.size();
        this->header.push_back(indexHeader);

        std::string timeHeader("Time");
        this->tPosition = this->header.size();
        this->header.push_back(timeHeader);

        std::string timeStepHeader("TimeStep");
        this->tStepPosition = this->header.size();
        this->header.push_back(timeStepHeader);

        std::vector<std::string> extCompHeader = this->gm->getExternalComponents()->getHeader();
        this->extComponentStartingPosition = this->header.size();
        for (std::vector<std::string>::iterator headIt = extCompHeader.begin(); headIt != extCompHeader.end(); ++headIt){
            this->header.push_back(*headIt);
        }

        for (std::vector<DYTSI_Modelling::Component*>::iterator it = this->componentList.begin();
                it != this->componentList.end(); ++it){
            DYTSI_Modelling::Component* comp = *it;
            std::vector<std::string> compHeader = comp->getHeader();
            this->componentsStartingPosition.push_back(this->header.size());
            for (std::vector<std::string>::iterator headIt = compHeader.begin(); headIt != compHeader.end(); ++headIt){
                this->header.push_back(*headIt);
            }
        }
    }
}

