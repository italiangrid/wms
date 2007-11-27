#  Copyright (c) Members of the EGEE Collaboration. 2004. 
#  See http://www.eu-egee.org/partners/ for details on the copyright
#  holders.  
#
#  Licensed under the Apache License, Version 2.0 (the "License"); 
#  you may not use this file except in compliance with the License. 
#  You may obtain a copy of the License at 
#
#      http://www.apache.org/licenses/LICENSE-2.0 
#
#  Unless required by applicable law or agreed to in writing, software 
#  distributed under the License is distributed on an "AS IS" BASIS, 
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
#  See the License for the specific language governing permissions and 
#  limitations under the License.
#
#  $Id$

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([org.glite.wms.common], [3.1.17], [])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_BOOST
AC_GLOBUS
AC_CPPUNIT
GLITE_BASE
GLITE_CHECK_CLASSADS
GLITE_CHECK_JOBID
GLITE_CHECK_WMSUTILS_CLASSADS
GLITE_CHECK_WMSUTILS_EXCEPTION

AC_PREFIX_DEFAULT([/opt/glite])

AC_CONFIG_FILES([Makefile
                 interface/Makefile
                 src/Makefile
                 src/configuration/Makefile
                 src/logger/Makefile
                 src/scripts/Makefile
                 src/utilities/Makefile
                 test/Makefile])
AC_OUTPUT

