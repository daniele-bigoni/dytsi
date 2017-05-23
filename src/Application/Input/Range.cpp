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
 * File:   Range.cpp
 * Author: s083781
 * 
 * Created on March 16, 2011, 11:13 AM
 */

#include "Range.h"

namespace DYTSI_Input {
    Range::Range() {
        this->startValue = 0.0;
        this->endValue = 0.0;
        this->stepSize = 0.0;
    }

    double Range::getStartValue() const{
        return this->startValue;
    }

    double Range::getEndValue() const{
        return this->endValue;
    }

    double Range::getStepSize() const{
        return this->stepSize;
    }
    
    double Range::getCoeff() const{
		return this->coeff;
	}

    void * Range::getParams() const{
		return this->params;
	}

    void Range::setStartValue( double val ){
        this->startValue = val;
    }

    void Range::setEndValue( double val ){
        this->endValue = val;
    }

    void Range::setStepSize( double val ){
        this->stepSize = val;
    }

    void Range::setCoeff( double val ){
		this->coeff = val;
	}

    void Range::setParams( void * val ){
		this->params = val;
	}

    Range::~Range() {
    }
}
