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
AC_CONFIG_SRCDIR([acconfig.h])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-llog4cpp':
AC_CHECK_LIB([log4cpp], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h mntent.h netdb.h sys/time.h sys/vfs.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_HEADER_TIME
AC_STRUCT_TM
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_HEADER_STDC
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_GETMNTENT
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([bzero endpwent ftruncate getmntent gettimeofday memset mkdir pow strcasecmp strerror strtol])

AC_BOOST
AC_CLASSADS
AC_GLOBUS
AC_CPPUNIT
AC_GLITE
AC_GLITE_WMSUTILS

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
