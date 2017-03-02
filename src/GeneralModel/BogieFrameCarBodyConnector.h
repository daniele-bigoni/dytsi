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
 * File:   BogieFrameCarBodyConnector.h
 * Author: bigo
 *
 * Created on February 12, 2011, 10:53 AM
 */

#ifndef BOGIEFRAMECARBODYCONNECTOR_H
#define	BOGIEFRAMECARBODYCONNECTOR_H

#include "Connector.h"

#include <string>

namespace DYTSI_Modelling {
    class Component;

    class BogieFrameCarBodyConnector : public Connector {
    public:
        BogieFrameCarBodyConnector( std::string name,
                Component* first, Component* second,
                RelativeComponentPosition posFirst,
                RelativeComponentPosition posSecond);
        virtual ~BogieFrameCarBodyConnector();

    private:
        
    };
}

#endif	/* BOGIEFRAMECARBODYCONNECTOR_H */

