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
#       Revision 1.13.2.13.4.1  2011/03/10 15:11:03  pandreet
#       Removed with-glite-location
#
#       Revision 1.13.2.13  2008/01/31 11:21:13  amarasch
#       version increased to 3.1.11 for bug #31278
#
#       Revision 1.13.2.12  2007/07/16 11:49:35  rlops
#       increased version
#
#       Revision 1.13.2.11  2007/07/10 10:53:18  rlops
#       increased version
#
#       Revision 1.13.2.10  2007/07/04 10:18:02  rlops
#       increased version
#
#       Revision 1.13.2.9  2007/06/05 09:28:45  rlops
#       decreased version
#
#       Revision 1.13.2.8  2007/06/05 09:26:57  rlops
#       increased version
#
#       Revision 1.13.2.7  2007/06/05 08:21:35  eronchie
#       Fix for bug #26705
#
#       Revision 1.13.2.6  2007/05/16 10:14:04  rlops
#       increased version
#
#       Revision 1.13.2.5  2007/03/30 13:05:12  rlops
#       increased version
#
#       Revision 1.13.2.4  2006/12/12 13:34:07  rlops
#       increased version
#
#       Revision 1.13.2.3  2006/11/21 16:18:38  eronchie
#       Increased version
#
#       Revision 1.13.2.2  2006/09/22 07:40:01  eronchie
#       increased version
#
#       Revision 1.13.2.1  2006/06/27 12:18:05  rlops
#       increased version
#
#       Revision 1.13  2006/03/20 22:06:07  eronchie
#       Increased version
#
#       Revision 1.12  2005/11/08 11:22:45  amarasch
#       version updated for new Tag
#
#       Revision 1.11  2005/10/15 11:55:27  eronchie
#       Increased version
#
#       Revision 1.10  2005/09/20 14:59:45  eronchie
#       Started merging with rc1.4
#
#       Revision 1.9  2005/02/22 13:36:29  eronchie
#       Updated version
#
#       Revision 1.8  2005/02/22 13:17:21  eronchie
#       Updated
#       Increased version due to the change in the glite-wms-parse-configuration.sh.in
#
#       Revision 1.7  2004/12/10 07:47:03  eronchie
#       Increased version
#
#       Revision 1.6  2004/10/19 12:01:15  eronchie
#       Removed config.h handling
#
#       Revision 1.5  2004/10/15 14:41:22  vmedici
#       start updating configuration according to dev-guide
#
#       Revision 1.4  2004/09/22 01:01:01  dimeglio
#       Fixed missing parenthesis in AC_INIT
#
#       Revision 1.3  2004/09/21 19:23:47  glbuild
#       modified module.version
#
#       Revision 1.2  2004/08/16 15:07:55  eronchie
#       Updated
#
#       Revision 1.1  2004/08/16 14:18:11  eronchie
#       Start adding
#       WARNING: work in progress
#
#
                                                                                                                                                             
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS Configuration], [3.1.11])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([config/glite_wms.conf])

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

have_condorg=no

AC_CONDORG(6.7.1, have_condorg=yes, have_condorg=no)
AC_MSG_RESULT(["CONDORG found $have_condorg"])

if test "$have_condorg" = "no"; then
  AC_MSG_RESULT(["Please verify CONDORG package"])
fi

if test "$have_condorg" = "no"; then
  AC_MSG_ERROR([Cannot continue building])
fi

GLITE_CHECK_LIBDIR

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([config/Makefile])
AC_CONFIG_FILES([config/glite-wms-check-daemons.sh])
AC_CONFIG_FILES([config/glite-wms-config.sh])
AC_CONFIG_FILES([config/glite-wms.csh])
AC_CONFIG_FILES([config/glite-wms-parse-configuration.sh])
AC_CONFIG_FILES([config/glite-wms.sh])
AC_CONFIG_FILES([config/glite-wms-vars.csh])
AC_CONFIG_FILES([config/glite-wms-vars.sh])

AC_OUTPUT

