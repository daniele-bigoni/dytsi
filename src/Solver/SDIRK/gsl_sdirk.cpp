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

#include "../gsl_odeiv_ext.h"
#include "sdirk.h"

static const gsl_odeiv2_step_type sdirk_type = {
  "SDIRK",                      /* name */
  1,                            /* can use dydt_in */
  0,                            /* gives exact dydt_out */
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

const gsl_odeiv2_step_type *gsl_odeiv2_step_sdirk = &sdirk_type;
