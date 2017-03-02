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

#include "Simulation.h"

#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define RELEASE_NUMBER 2

#ifdef GRAPHICS
#include <QtGui/QApplication>
#include <qtconcurrentrun.h>
#include "mainwindow.h"
#endif

int RunningMode_graphics;
int RunningMode_threads;

int main(int argc, char *argv[])
{
    printf("DYTSI version %d.%dr%d compiled on " __DATE__ " at " __TIME__ ".\n", MAJOR_VERSION, MINOR_VERSION, RELEASE_NUMBER);

    // Check if it is running in text mode or graphic mode
    if (argc >= 2){
        if (strcmp(argv[1],"-G")==0){
	  #ifdef GRAPHICS
            // Run Graphic mode
            QApplication a(argc, argv);
            Simulation sim = *(new Simulation());
            MainWindow w;
            w.setSimulation(sim);
            w.show();

            return a.exec();
            RunningMode_graphics = true;
	  #else
	    printf("DYTSI Error: Graphics are not supported in this built version.\n");
	  #endif
        } else if (strcmp(argv[1],"-T")==0){
            // Run Text Mode
            RunningMode_graphics = false;
            Simulation(argc, argv);
            exit(0);
        } else {
            fprintf(stderr,"Wrong flag input. Usage: ./dytsi [-T/-G] [...]");
        }
    }

}
