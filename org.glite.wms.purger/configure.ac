#       Copyright (c) Members of the EGEE Collaboration. 2004. 
#       See http://www.eu-egee.org/partners/ for details on the copyright
#       holders.  
#       
#       Licensed under the Apache License, Version 2.0 (the "License"); 
#       you may not use this file except in compliance with the License. 
#       You may obtain a copy of the License at 
#       
#           http://www.apache.org/licenses/LICENSE-2.0 
#       
#       Unless required by applicable law or agreed to in writing, software 
#       distributed under the License is distributed on an "AS IS" BASIS, 
#       WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
#       See the License for the specific language governing permissions and 
#       limitations under the License.
#
# $Id$

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS Purger], [3.1.2])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/purger.cpp])

# Notices.
AC_COPYRIGHT([Copyright (c) 2004 The EU EGEE Project
See LICENCE file for details
])
AC_REVISION([$Revision$])

#Environment.
WORKDIR=`pwd`
AC_SUBST(WORKDIR)

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h mntent.h sys/vfs.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_TYPE_UID_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_HEADER_STDC
AC_FUNC_ERROR_AT_LINE
AC_FUNC_GETMNTENT
AC_FUNC_MEMCMP
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([bzero endpwent ftruncate getmntent memset mkdir pow strerror strtol])

have_boost=no
have_classads=no
have_globus=no

AC_BOOST([], have_boost=yes, have_boost=no)
AC_MSG_RESULT(["BOOST found $have_boost"])

AC_CLASSADS([], have_classads=yes, have_classads=no)
AC_MSG_RESULT(["CLASSADS found $have_classads"])

AC_GLOBUS([], have_globus=yes, have_globus=no)
AC_MSG_RESULT(["GLOBUS found $have_globus"])

if test "$have_globus" = "no"; then
  AC_MSG_RESULT(["Please verify GLOBUS package"])
fi

if test "$have_boost" = "no"; then
  AC_MSG_RESULT(["Please verify BOOST package"])
fi

if test "$have_classads" = "no"; then
  AC_MSG_RESULT(["Please verify CLASSADS package"])
fi

if test "$have_globus" = "no"; then
  AC_MSG_ERROR([Cannot continue building])
fi

if test "$have_boost" = "no"; then
  AC_MSG_ERROR([Cannot continue building])
fi

if test "$have_classads" = "no"; then
  AC_MSG_ERROR([Cannot continue building])
fi

have_glite_wms_common=no
have_glite_wmsutils=no
have_glite_lb=no

AC_GLITE
AC_OPTIMIZE

AC_GLITE_WMS_COMMON([], have_glite_wms_common=yes, have_glite_wms_common=no)
AC_MSG_RESULT(["GLITE_WMS_COMMON found $have_glite_wms_common"])

AC_GLITE_WMSUTILS([], have_glite_wmsutils=yes, have_glite_wmsutils=no)
AC_MSG_RESULT(["GLITE_WMSUTILS found $have_glite_wmsutils"])

AC_GLITE_LB([], have_glite_lb=yes, have_glite_lb=no)
AC_MSG_RESULT(["GLITE_LB found $have_glite_lb"])

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])

AC_OUTPUT

