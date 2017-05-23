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
 * File:   Bifurcation.h
 * Author: bigo
 *
 * Created on March 11, 2011, 5:50 PM
 */

#ifndef BIFURCATION_H
#define	BIFURCATION_H

#include "Input.h"
#include "Range.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <vector>

namespace DYTSI_Input {

    class Bifurcation : public Input{
    public:
        enum StartingValuesUsage {SV_FIRST, SV_ALL};

        Bifurcation( DYTSI_Input::InputHandler* inputHandler, XERCES_CPP_NAMESPACE::DOMElement* node, int& error);
        virtual ~Bifurcation();

        DYTSI_Solver::Solution* simulate();
    private:
        StartingValuesUsage startingValuesUsage;
        Range* tSpan;
        Range* speedRange;
        Range* radiusRange;
        Range* Phi_seRange;
    };
}

#endif	/* BIFURCATION_H */

