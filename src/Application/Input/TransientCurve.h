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
 * File:   TransientCurve.h
 * Author: bigo
 *
 * Created on May 7, 2011, 12:03 PM
 */

#ifndef TRANSIENTCURVE_H
#define	TRANSIENTCURVE_H

#include <xercesc/dom/DOM.hpp>

namespace DYTSI_Input {
    class TransientCurve {
    public:
        TransientCurve();
        virtual ~TransientCurve();

        void setTransientRadius(XERCES_CPP_NAMESPACE::DOMElement* el, int errCode);
        void setTransientCant(XERCES_CPP_NAMESPACE::DOMElement* el, int errCode);

        bool isSettedRadius(){ return settedRadius; }
        bool isSettedCant(){ return settedCant; }

        double getRadiusStart(){ return radiusStart; }
        double getRadiusEnd(){ return radiusEnd; }
        double getRadiusTime(){ return radiusTime; }

        double getCantStart(){ return cantStart; }
        double getCantEnd(){ return cantEnd; }
        double getCantTime(){ return cantTime; }
        
    private:
        bool settedRadius;
        bool settedCant;

        double radiusStart;
        double radiusEnd;
        double radiusTime;

        double cantStart;
        double cantEnd;
        double cantTime;

    };
}

#endif	/* TRANSIENTCURVE_H */

