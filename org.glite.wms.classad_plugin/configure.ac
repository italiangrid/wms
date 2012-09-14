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
#       Revision 1.13  2006/09/19 15:44:39  monforte
#       Merge against 3.1 and code review
#
#       Revision 1.12.2.1  2006/06/22 06:13:04  monforte
#       Removed FQAN/ACL check functions from gangmatch plugin library.
#       Added FQAN/ACL check functions as indipendent plugin library.
#
#       Revision 1.12  2006/03/20 21:53:33  eronchie
#       Increased version
#
#       Revision 1.11  2005/10/15 11:52:57  eronchie
#       Decreased version
#
#       Revision 1.10  2005/09/30 18:32:47  fprelz
#       Added LB reputability 'successFraction' classad function.
#
#       Revision 1.9  2005/09/30 14:23:02  fprelz
#       Initial addition of classad function support for LB reputability rank.
#
#       Revision 1.8  2005/09/21 13:29:28  monforte
#       Merge against glite-wms-classad_plugin_branch_1_0_0
#
#       Revision 1.7.2.1  2005/04/11 11:02:30  eronchie
#       Added optimize.m4
#       Added --disable-optimization
#       Added AC_OPTIMIZE
#
#       Revision 1.7  2004/12/10 07:45:27  eronchie
#       Increased version
#
#       Revision 1.6  2004/09/22 01:00:03  dimeglio
#       Fixed missing parenthesis in AC_INIT
#
#       Revision 1.5  2004/09/21 19:24:26  glbuild
#       modified module.version
#
#       Revision 1.4  2004/07/23 16:30:58  eronchie
#       The component name is wms-classad_plugin
#
#       Revision 1.3  2004/06/26 13:38:37  eronchie
#       *** empty log message ***
#
#       Revision 1.2  2004/06/26 13:24:23  eronchie
#       Start adding classad_plugin
#       WARNING: work in progress
#
#       Revision 1.1  2004/06/26 13:00:44  eronchie
#       Start adding classad_plugin
#       WARNING: work in progress
#
#
                                                                                                                                                             
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS ClassAd_Plugin], [3.1.0])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/classad_plugin_loader.h])

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

AC_BOOST([], have_boost=yes, have_boost=no)
AC_CLASSADS([], have_classads=yes, have_classads=no)

AC_MSG_RESULT(["CLASSADS found $have_classads"])
AC_MSG_RESULT(["BOOST found $have_boost"])

have_glite_wms_common=no

AC_OPTIMIZE
AC_GLITE

AC_GLITE_WMS_COMMON([], have_glite_wms_common=yes, have_glite_wms_common=no)
AC_MSG_RESULT(["GLITE_WMS_COMMON found $have_glite_wms_common"])

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/gangmatch/Makefile])
AC_CONFIG_FILES([src/lb_rank/Makefile])
AC_CONFIG_FILES([src/fqan/Makefile])
AC_OUTPUT

