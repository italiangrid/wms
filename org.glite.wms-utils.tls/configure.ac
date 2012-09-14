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
#       Common configure.ac file for the GLite WMS Common module
#
#       Authors: Alberto Di Meglio <alberto.di.meglio@cern.ch>
#       Version info: $Id$
#       Release: $Name$
#
#       Revision history:
#       $Log$
#       Revision 1.24  2007/07/03 14:36:47  eronchie
#       *** empty log message ***
#
#       Revision 1.23  2007/07/03 14:27:50  eronchie
#       Merge against glite-wms-utils-tls_branch_3_1_0
#
#       Revision 1.22  2006/06/26 21:19:10  eronchie
#       Increased version
#       Fix bug 17601
#
#       Revision 1.21  2006/03/20 15:31:38  eronchie
#       Increased version
#
#       Revision 1.20  2005/10/15 07:53:29  eronchie
#       Increased version
#
#       Revision 1.19  2005/09/20 10:59:57  eronchie
#       Started merging with rc1.4
#
#       Revision 1.18  2004/12/10 07:34:52  eronchie
#       Increased version
#
#       Revision 1.17  2004/11/16 15:33:16  eronchie
#       Increased version
#
#       Revision 1.16  2004/09/22 00:25:17  glbuild
#       Fixed missing parenthesis
#
#       Revision 1.15  2004/09/21 19:22:03  glbuild
#       modified module.version
#
#       Revision 1.14  2004/08/17 10:38:34  eronchie
#       Added optimize macro
#
#       Revision 1.13  2004/08/04 12:15:39  eronchie
#       Added AC_CPPUNIT
#       CPPUNIT_CFLAGS
#       CPPUNIT_LIBS
#
#       Revision 1.12  2004/08/04 10:10:42  eronchie
#       Start adding unittest
#
#       Revision 1.11  2004/07/28 07:25:04  eronchie
#       Added m4files.list property
#
#       Revision 1.10  2004/07/27 09:13:49  eronchie
#       Removed AC_WMSUTILS_EXCEPTION and AC_GLOBUS_SSL_UTILS checks
#       Set directly exception library macro
#
#       Revision 1.9  2004/07/21 17:35:43  eronchie
#       Moved some tests in examples
#       Removed files from test
#
#       Revision 1.8  2004/07/21 17:24:34  eronchie
#       Moved out tls from org.glite.wms and put in org.glite.wms-utils
#
#
                                                                                                                                                             
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS Utils Tls], [3.1.3])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/ssl_helpers/ssl_inits.h])

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

have_globus=no
have_cppunit=no

AC_GLOBUS([], have_globus=yes, have_globus=no)
AC_MSG_RESULT(["GLOBUS found $have_globus"])

AC_CPPUNIT([], have_cppunit=yes, have_cppunit=no)
AC_MSG_RESULT(["CPPUNIT found $have_cppunit"])

if test "$have_globus" = "no"; then
  AC_MSG_RESULT(["Please verify GLOBUS package"])
fi

if test "$have_cppunit" = "no"; then
  AC_MSG_RESULT(["Please verify CPPUNIT package"])
fi

if test "$have_globus" = "no"; then
  AC_MSG_ERROR([Cannot continue building])
fi

if test "$have_cppunit" = "no"; then
  AC_MSG_ERROR([Cannot continue building])
fi

AC_GLITE

GLITE_WMSUTILS_EXCEPTION_LIBS="-L$GLITE_LOCATION/lib -lglite_wmsutils_exception"

AC_SUBST(GLITE_WMSUTILS_EXCEPTION_LIBS)

AC_OPTIMIZE

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([interface/Makefile])
AC_CONFIG_FILES([examples/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/socket++/Makefile])
AC_CONFIG_FILES([src/ssl_helpers/Makefile])
AC_CONFIG_FILES([test/Makefile])

AC_OUTPUT

