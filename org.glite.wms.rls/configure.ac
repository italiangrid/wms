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
#       Revision 1.14  2007/03/26 13:23:24  emartel
#       removed Storage Index client
#
#       Revision 1.13  2007/03/05 11:44:11  mcecchi
#       lb dependencies newly carried by by wms_utils
#
#       Revision 1.12  2007/02/21 16:21:39  emartel
#       removed edg-replica catalog
#
#       Revision 1.11  2006/11/22 15:35:12  emartel
#       added StorageIndex and DLI catalog interfaces
#
#       Revision 1.10  2006/06/27 12:12:59  rlops
#       increased version
#
#       Revision 1.9  2006/03/20 22:54:38  eronchie
#       Increased version
#
#       Revision 1.8  2005/10/15 12:21:34  eronchie
#       Increased version
#
#       Revision 1.7  2004/12/10 07:53:01  eronchie
#       Increased version
#
#       Revision 1.6  2004/10/17 15:11:42  eronchie
#       ..... Removed _BASE
#
#       Revision 1.5  2004/10/12 12:22:59  eronchie
#       Updated
#
#       Revision 1.4  2004/10/06 14:32:49  eronchie
#       Start adding rls
#
#       Revision 1.3  2004/10/06 10:53:54  eronchie
#       Updated
#       Added REPLICA
#
#       Revision 1.2  2004/10/05 13:34:41  monforte
#       *** WORK IN PROGRESS ***
#
#       Revision 1.1  2004/10/05 11:08:04  monforte
#       Initial porting of RLS: *** WORK IN PROGRESS ***
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
AC_INIT([GLite WMS RLS], [3.1.1])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
#AC_CONFIG_SRCDIR([src/ReplicaServiceException.h])

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
have_globus=no
have_gsoap=no
have_classads=no
have_glite_lb=no

AC_BOOST([], have_boost=yes, have_boost=no)
AC_GLOBUS([], have_globus=yes, have_globus=no)
AC_GSOAP([], have_gsoap=yes, have_gsoap=no)
AC_CLASSADS([], have_classads=yes, have_classads=no)
AC_GLITE_LB([], have_glite_lb=yes, have_glite_lb=no)


AC_MSG_RESULT(["BOOST found $have_boost"])
AC_MSG_RESULT(["GSOAP found $have_gsoap"])
AC_MSG_RESULT(["GLOBUS found $have_globus"])
AC_MSG_RESULT(["CLASSADS found $have_classads"])
AC_MSG_RESULT(["GLITE_LB found $have_glite_lb"])

AC_GLITE
AC_SEC_GSOAP_PLUGIN
AC_GLITE_SERVICE_DISCOVERY
AC_GLITE_JDL
AC_GLITE_WMSUTILS
AC_GLITE_WMS_COMMON
AC_OPTIMIZE


# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([interface/Makefile])
AC_CONFIG_FILES([src/DLI/Makefile])

AC_OUTPUT

