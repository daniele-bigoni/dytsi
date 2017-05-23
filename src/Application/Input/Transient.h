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
 * File:   Transient.h
 * Author: bigo
 *
 * Created on March 11, 2011, 1:46 PM
 */

#ifndef TRANSIENT_H
#define	TRANSIENT_H

#include "Input.h"
#include "Range.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <vector>

namespace DYTSI_Input {

    class Transient : public Input {
    public:
        Transient( DYTSI_Input::InputHandler* inputHandler, XERCES_CPP_NAMESPACE::DOMElement* el, int& error);
        virtual ~Transient();

        DYTSI_Solver::Solution* simulate();
    private:
        Range* tSpan;
        double speed;
        double radius;
        double Phi_se;

        void setStartingValues(XERCES_CPP_NAMESPACE::DOMNode* node);
    };
}

#endif	/* TRANSIENT_H */

