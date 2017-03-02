=====================================================

 DYTSI - DYnamics Train SImulator

 Author: Daniele Bigoni (dabi@dtu.dk)

=====================================================

0. REQUIREMENTS
1. INSTALLATION
2. USAGE
3. LICENSE

=======================================================
0. REQUIREMENTS
   GNU Scientific Library (GSL) >= 1.15
   Xerces-C >= 3.1.1
   Boost >= 1.46.1 [filesystem,system]

=======================================================
1. INSTALLATION
 In the following <path> is the target path of installation and 
 [n-jobs] is the number of threads to be used to compile. Both
 of them can be omitted and default values will be used.

 $ tar xzf dytsi-1.0.tar.gz
 $ cd dytsi-1.0
 $ mkdir build
 $ cd build
 $ ../configure --prefix=<path>
 $ make -j[n-jobs]
 $ make install

=======================================================
3. USAGE
Example input files for DYTSI are provided in the folder input.
Check that the rsgeo files defined in the input files are in the correct location in the file system and that you made the appropriate settings regarding the output location.
Run:

 $ DYTSI -T -nt <inputFile>.xml

=======================================================
3. LICENSE

This file is part of DYTSI.

DYTSI is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DYTSI is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DYTSI.  If not, see <http://www.gnu.org/licenses/>.

DYnamics Train SImulation (DYTSI)
Copyright (C) 2011-2014 The Technical University of Denmark
Scientific Computing Section
Department of Applied Mathematics and Computer Science

Author: Daniele Bigoni
E-mail: dabi@dtu.dk
