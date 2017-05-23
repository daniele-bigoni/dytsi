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
 * File:   RSGEOTableReader.h
 * Author: s083781
 *
 * Created on May 27, 2011, 12:27 PM
 */

#ifndef RSGEOTABLEREADER_H
#define	RSGEOTABLEREADER_H

#include <stdio.h>
#include <string>
#include <vector>

#include "XMLaux.h"

#include "../../GeneralModel/RSGEOTable.h"

namespace DYTSI_Input{
    class RSGEOTableReader {
    public:
        RSGEOTableReader();
        virtual ~RSGEOTableReader();

        static void read(std::vector<DYTSI_Modelling::RSGEOTable*>& rsgList, std::string path, char tabDelimiter);
        static void read(std::vector<DYTSI_Modelling::RSGEOTable*>& rsgList, XERCES_CPP_NAMESPACE::DOMNode* node, char tabDelimiter);
    private:

    };
}

#endif	/* RSGEOTABLEREADER_H */

