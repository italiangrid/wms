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
#       Revision 1.10  2006/03/17 07:24:07  eronchie
#       Updated
#
#       Revision 1.9  2006/03/13 14:35:59  monforte
#       Merge against glite-wms-broker_branch_1_5_0_RC15x
#
#       Revision 1.8.2.5  2006/02/02 18:38:21  fprelz
#       Adding matchmaking performance patches (as tested by ECGI at end 2005 on v1.4)
#
#       Revision 1.8.2.4  2006/01/13 16:38:20  monforte
#       fix for bug #14497
#
#       Revision 1.8.2.3  2005/12/23 09:03:58  monforte
#       Fix to bug #14370, Fuzzy rank doesn't work
#
#       Revision 1.8.2.2  2005/11/18 14:28:46  monforte
#       Fix for bug #13782: "WMS seg-fault if fuzzy-rank is activated"
#
#       Revision 1.8.2.1  2005/10/28 10:57:24  monforte
#       Fixed a problem with fuzzy ranking
#
#       Revision 1.8  2005/10/15 11:44:01  eronchie
#       Decreased version to 1 5 0
#
#       Revision 1.7  2005/09/21 14:19:46  monforte
#       Merge against glite-wms-broker_branch_1_0_0
#
#       Revision 1.6  2004/12/10 07:40:38  eronchie
#       Increased version
#
#       Revision 1.5  2004/09/22 00:58:44  dimeglio
#       Fixed missing parenthesis in AC_INIT
#
#       Revision 1.4  2004/09/21 19:24:39  glbuild
#       modified module.version
#
#       Revision 1.3  2004/07/28 17:23:17  eronchie
#       merd
#
#       Revision 1.2  2004/07/10 07:01:57  eronchie
#       Updated configure.ac
#       Removed wrong CFLAGS
#
#       Revision 1.1  2004/07/02 08:48:27  monforte
#       Initial Broker porting to new build system
#       WARNING: work in progress...
#
#       Revision 1.1  2004/06/26 15:25:03  eronchie
#       Start handling matchmaking
#       WARNING: work in progress
#
#
#
                                                                                                                                                             
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS Broker], [3.1.1])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/ResourceBroker.h])

# Notices.
AC_COPYRIGHT([Copyright (c) 2004 The EU EGEE Project
See LICENCE file for details
])
AC_REVISION([$Revision$])

#Environment.
WORKDIR=`pwd`
AC_SUBST(WORKDIR)

AC_OPTIMIZE

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
AC_CLASSADS([], have_classads=yes, have_classads=no)
AC_GLOBUS([], have_globus=yes, have_globus=no)

AC_MSG_RESULT(["CLASSADS found $have_classads"])
AC_MSG_RESULT(["BOOST found $have_boost"])
AC_MSG_RESULT(["GLOBUS found $have_globus"])

have_glite_jdl=no
have_glite_wms_common=no

AC_GLITE
AC_GLITE_WMS_RLS
AC_GLITE_SERVICE_DISCOVERY
AC_GLITE_WMSUTILS
AC_GLITE_WMS_ISM
AC_GLITE_JDL([],have_glite_jdl=yes,have_glite_jdl=no)
AC_GLITE_WMS_COMMON([],have_glite_wms_common=yes,have_glite_wms_common=no)

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_OUTPUT

