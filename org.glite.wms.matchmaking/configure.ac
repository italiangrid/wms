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

#       Revision 1.12  2006/03/20 22:47:54  eronchie
#       Increased version
#
#       Revision 1.11  2006/03/13 14:38:05  monforte
#       Merge against glite-wms-matchmaking_branch_1_5_0_RC15
#
#       Revision 1.10.2.4  2006/02/02 18:41:32  fprelz
#       Adding matchmaking performance patches (as tested by ECGI at end 2005 on v1.4)
#
#       Revision 1.10.2.3  2006/01/20 00:25:57  eronchie
#       Increased version
#
#       Revision 1.10.2.2  2005/11/03 14:04:35  eronchie
#       Increased version
#
#       Revision 1.10.2.1  2005/10/28 11:03:56  monforte
#       removed search in ism while ranking
#
#       Revision 1.10  2005/10/15 12:16:26  eronchie
#       Increased version
#
#       Revision 1.9  2005/09/21 13:33:25  monforte
#       Merge against glite-wms-matchmaking_branch_1_1_0
#
#       Revision 1.8  2005/06/16 08:17:49  monforte
#       *** empty log message ***
#
#       Revision 1.6.2.4  2005/05/05 12:23:54  monforte
#       Integration with CEMON 1.2
#
#       Revision 1.6.2.3  2005/04/11 10:49:32  eronchie
#       Added AC_OPTIMIZE
#       Added optimize.m4
#       Added --disable-optimization
#
#       Revision 1.6.2.2  2005/02/08 15:09:07  fprelz
#       Increased minor version because of thread safety bug fix in candidate
#       CE selection.
#
#       Revision 1.6.2.1  2005/01/20 15:46:45  eronchie
#       Updated
#       Bug fix for expiry ism entries
#
#       Revision 1.6  2004/12/10 07:51:43  eronchie
#       Increased version
#
#       Revision 1.5  2004/09/22 01:08:24  dimeglio
#       Fixed missing parenthesis in AC_INIT
#
#       Revision 1.4  2004/09/21 19:24:33  glbuild
#       modified module.version
#
#       Revision 1.3  2004/07/10 06:44:36  eronchie
#       Removed AC_GLITE_WMS_COMMON
#       It is not necessary
#
#       Revision 1.2  2004/06/30 08:35:15  monforte
#       Initial MatchMaking porting to new build system
#
#       Revision 1.1  2004/06/26 15:25:03  eronchie
#       Start handling matchmaking
#       WARNING: work in progress
#
#
#
                                                                                                                                                             
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS Matchmaking], [3.1.1])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/utility.h])

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

AC_GLITE
AC_OPTIMIZE

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([interface/Makefile])
AC_OUTPUT

