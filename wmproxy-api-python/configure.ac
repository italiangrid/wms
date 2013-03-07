#
#       Copyright (c) 2004 on behalf of the EU EGEE Project:
#       The European Organization for Nuclear Research (CERN),
#       Istituto Nazionale di Fisica Nucleare (INFN), Italy
#       Datamat Spa, Italy
#       Centre National de la Recherche Scientifique (CNRS), France
#       CS Systeme d'Information (CSSI), France
#       Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
#       Universiteit van Amsterdam (UvA), Netherlands
#       University of Helsinki (UH.HIP), Finland
#       University of Bergen (UiB), Norway
#       Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom
#
#       Authors: Alessandro Maraschini <alessandro.maraschini@datamat.it>
#

AC_PREREQ(2.57)
AC_INIT([GLite WMS wmproxy-api-python], [3.3.3])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/wmproxymethods.py])
#AC_CONFIG_SRCDIR([examples/make_proxy_cert.c])

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
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([memset])
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([ftruncate localtime_r pow strdup strerror strstr])

GLITE_CHECK_LIBDIR

AC_PYTHON([2.2.3], AC_MSG_RESULT(["PYTHON ok"]), AC_MSG_ERROR(["PYTHON not found"]))

#AC_GRIDSITE([], AC_MSG_RESULT(["GRIDSITE ok"]), AC_MSG_ERROR(["GRIDSITE not found"]))
#AC_GLOBUS([], AC_MSG_RESULT(["GLOBUS ok"]), AC_MSG_ERROR(["GLOBUS not found"]))


# Configuration items
AC_PREFIX_DEFAULT([/usr])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
#AC_CONFIG_FILES([examples/Makefile])

AC_OUTPUT


