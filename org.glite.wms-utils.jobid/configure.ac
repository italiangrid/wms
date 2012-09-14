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
#       Revision 1.20  2006/06/26 21:17:11  eronchie
#       Increased version
#       Fix bug 17601
#
#       Revision 1.19  2006/03/20 15:30:43  eronchie
#       Increased version
#
#       Revision 1.18  2005/10/15 07:55:00  eronchie
#       Increased version
#
#       Revision 1.17  2005/09/20 10:41:49  eronchie
#       Started merging with rc1.4
#
#       Revision 1.16  2004/12/10 07:33:42  eronchie
#       Increased version
#
#       Revision 1.15  2004/11/16 15:35:22  eronchie
#       Increased version
#
#       Revision 1.14  2004/11/16 15:15:45  eronchie
#       Increased version
#
#       Revision 1.13  2004/09/22 00:25:17  glbuild
#       Fixed missing parenthesis
#
#       Revision 1.12  2004/09/21 19:22:09  glbuild
#       modified module.version
#
#       Revision 1.11  2004/08/17 13:46:28  eronchie
#       Added interface
#
#       Revision 1.10  2004/08/17 13:41:20  eronchie
#       Moved out JobIdExceptions.h cjobid.h JobId.h
#       Put in interface/glite/wmsutils/jobid
#
#       Revision 1.9  2004/08/17 10:40:34  eronchie
#       Added optimize option
#
#       Revision 1.8  2004/07/27 09:14:19  eronchie
#       Removed AC_WMSUTILS_EXCEPTION and AC_GLOBUS_SSL_UTILS checks
#       Set directly exception library macro
#
#       Revision 1.7  2004/07/21 17:53:36  eronchie
#       Moved out org.glite.wms.jobid from org.glite.wms and put in org.glite.wms-utils
#
#
#
                                                                                                                                                             
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS Utils Jobid], [3.1.1])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/jobid/strmd5.h])

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

AC_GLOBUS([], have_globus=yes, have_globus=no)
AC_MSG_RESULT(["GLOBUS found $have_globus"])

if test "$have_globus" = "no"; then
  AC_MSG_RESULT(["Please verify GLOBUS package"])
fi

if test "$have_globus" = "no"; then
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
AC_CONFIG_FILES([examples/Makefile])
AC_CONFIG_FILES([interface/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/jobid/Makefile])

AC_OUTPUT

