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
 * File:   PrintingHandler.h
 * Author: s083781
 *
 * Created on February 9, 2011, 12:51 PM
 */

#ifndef PRINTINGHANDLER_H
#define	PRINTINGHANDLER_H

#include <sstream>

class PrintingHandler {
public:
    PrintingHandler();
    virtual ~PrintingHandler();

    enum Terminal {TEXT};
    enum OutputType {STDOUT, STDERR};

    static void printOut(char* output, OutputType type);
    static void printOut(std::stringstream* output, OutputType type);
private:

};

#endif	/* OUTPUTHANDLER_H */

