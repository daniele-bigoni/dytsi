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
 * File:   TransientCurve.cpp
 * Author: bigo
 * 
 * Created on May 7, 2011, 12:03 PM
 */

#include "TransientCurve.h"

#include "XMLaux.h"

namespace DYTSI_Input {
    TransientCurve::TransientCurve() {
        this->settedRadius = false;
        this->settedCant = false;
    }

    TransientCurve::~TransientCurve() {
    }

    void TransientCurve::setTransientRadius(xercesc_3_1::DOMElement* el, int errCode){
        errCode += XMLaux::loadDoubleProperty(el,"StartValue", this->radiusStart);
        errCode += XMLaux::loadDoubleProperty(el,"EndValue", this->radiusEnd);
        errCode += XMLaux::loadDoubleProperty(el,"TransientTime", this->radiusTime);
        if (errCode == 0)
            this->settedRadius = true;
        else
            this->settedRadius = false;
    }

    void TransientCurve::setTransientCant(xercesc_3_1::DOMElement* el, int errCode){
        errCode += XMLaux::loadDoubleProperty(el,"StartValue", this->cantStart);
        errCode += XMLaux::loadDoubleProperty(el,"EndValue", this->cantEnd);
        errCode += XMLaux::loadDoubleProperty(el,"TransientTime", this->cantTime);
        if (errCode == 0)
            this->settedCant = true;
        else
            this->settedCant = false;
    }
}
