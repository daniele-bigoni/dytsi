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
 * File:   Thread.cpp
 * Author: s083781
 * 
 * Created on April 12, 2011, 9:16 AM
 */

#include "Thread.h"
#include <assert.h>

namespace DYTSI_Modelling {
    Thread::Thread() : m_running(false) {}

    int Thread::Start(void * arg)
    {
        if(m_running == false){
            m_running = true;
            Arg(arg); // store user data
            int code = pthread_create(& ThreadId_, NULL, Thread::EntryPoint, this);
            return code;
        } else
            return -1;
    }

    void Thread::Join(){
        if (m_running == true){
            m_running = false;
            pthread_join(ThreadId_, 0);
        }
    }

    int Thread::Run(void * arg)
    {
       Setup();
       Execute( arg );
    }

    /*static */
    void * Thread::EntryPoint(void * pthis)
    {
       Thread * pt = (Thread*)pthis;
       pt->Run( pt->Arg() );
    }

    void Thread::Setup()
    {
            // Do any setup here
    }
}

