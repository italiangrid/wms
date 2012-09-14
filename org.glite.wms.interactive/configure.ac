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
#       Version info: $Id $
#       Release: $Name$
#
#       Revision history:
#       $Log$
#       Revision 1.10  2007/11/13 12:21:09  mezzadri
#       lb libraries are now linked dinamically and also c-ares and expat are linked dinamically
#
#       Revision 1.9  2006/06/27 12:06:46  rlops
#       increased version
#
#       Revision 1.8  2006/03/20 22:19:30  eronchie
#       Increased version
#       Added optimize
#
#       Revision 1.7  2005/10/15 11:58:00  eronchie
#       Increased version
#
#       Revision 1.6  2004/12/09 13:57:59  mezzadri
#       changed version number
#
#       Revision 1.5  2004/09/22 01:01:55  dimeglio
#       Fixed missing parenthesis in AC_INIT
#
#       Revision 1.4  2004/09/21 19:23:59  glbuild
#       modified module.version
#
#       Revision 1.3  2004/08/18 08:07:23  eronchie
#       Added internal and external dependencies
#
#       Revision 1.2  2004/08/16 13:21:25  eronchie
#       Updated
#       WARNING: work in progress
#
#       Revision 1.1  2004/08/16 12:53:47  eronchie
#       Start adding new component
#       WARNING: work in progress
#
#
                                                                                                                                                             
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS Interactive], [3.1.1])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/glite_wms_pipe_input.c])

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
AC_CPPUNIT([], have_cppunit=yes, have_cppunit=no)

AC_MSG_RESULT(["GLOBUS found $have_globus"])
AC_MSG_RESULT(["CPPUNIT found $have_cppunit"])

have_tp_bypass=no
have_glite_wmsutils=no
have_glite_lb=no

AC_GLITE
AC_OPTIMIZE

AC_GLITE_WMS_TP_BYPASS([], have_tp_bypass=yes, have_tp_bypass=no)
AC_MSG_RESULT(["TP_BYPASS found $have_tp_bypass"])

AC_GLITE_LB([], have_glite_lb=yes, have_glite_lb=no)
AC_MSG_RESULT(["GLITE_LB found $have_glite_lb"])

AC_GLITE_WMSUTILS([], have_glite_wmsutils=yes, have_glite_wmsutils=no)
AC_MSG_RESULT(["GLITE_WMSUTILS found $have_glite_wmsutils"])

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/README])

AC_OUTPUT

