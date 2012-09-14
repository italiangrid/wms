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
#
#
                                                                                                                                                             
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS NS Daemon], [3.1.3])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/NetworkServerImpl.h])

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

AC_OPTIMIZE

have_boost=no
have_classads=no
have_globus=no
have_expat=no
have_libtar=no

AC_BOOST([], have_boost=yes, have_boost=no)
AC_MSG_RESULT(["BOOST found $have_boost"])

AC_CLASSADS([], have_classads=yes, have_classads=no)
AC_MSG_RESULT(["CLASSADS found $have_classads"])

AC_GLOBUS([], have_globus=yes, have_globus=no)
AC_MSG_RESULT(["GLOBUS found $have_globus"])

AC_EXPAT([], have_expat=yes, have_expat=no)
AC_MSG_RESULT(["EXPAT found $have_expat"])

AC_LIBTAR([], have_libtar=yes, have_libtar=no)
AC_MSG_RESULT(["LIBTAR found $have_libtar"])

if test "$have_globus" = "no"; then
  AC_MSG_RESULT(["Please verify GLOBUS package"])
fi

if test "$have_boost" = "no"; then
  AC_MSG_RESULT(["Please verify BOOST package"])
fi

if test "$have_classads" = "no"; then
  AC_MSG_RESULT(["Please verify CLASSADS package"])
fi

if test "$have_expat" = "no"; then
  AC_MSG_RESULT(["Please verify EXPAT package"])
fi

if test "$have_libtar" = "no"; then
  AC_MSG_RESULT(["Please verify LIBTAR package"])
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

if test "$have_expat" = "no"; then
  AC_MSG_ERROR([Cannot continue building])
fi

if test "$have_libtar" = "no"; then
  AC_MSG_ERROR([Cannot continue building])
fi

have_glite_wmsutils=no
have_glite_wms_common=no
have_glite_wms_brokerinfo=no
have_glite_wms_ism=no
have_glite_jp=no
have_glite_lb=no
have_glite_wms_ns_common=no
have_glite_wms_purger=no

AC_GLITE

AC_GLITE_WMS_COMMON([], have_glite_wms_common=yes, have_glite_wms_common=no)
AC_MSG_RESULT(["GLITE_WMS_COMMON found $have_glite_wms_common"])

AC_GLITE_WMSUTILS([], have_glite_wmsutils=yes, have_glite_wmsutils=no)
AC_MSG_RESULT("GLITE_WMSUTILS $have_glite_wmsutils")

AC_GLITE_LB([], have_glite_lb=yes, have_glite_lb=no)
AC_MSG_RESULT(["GLITE_LB found $have_glite_lb"])

AC_GLITE_JP([], have_glite_jp=yes, have_glite_jp=no)
AC_MSG_RESULT(["GLITE_JP found $have_glite_jp"])

AC_GLITE_JDL([], have_glite_jdl=yes, have_glite_jdl=no)
AC_MSG_RESULT(["GLITE_JDL found $have_glite_jdl"])

AC_GLITE_WMS_PURGER([], have_glite_wms_purger=yes, have_glite_wms_purger=no)
AC_MSG_RESULT(["GLITE_WMS_PURGER found $have_glite_wms_purger"])

AC_GLITE_WMS_CLASSAD_PLUGIN([], have_glite_wms_cp=yes, have_glite_wms_cp=no)
AC_MSG_RESULT(["GLITE_WMS_CLASSAD_PLUGIN found $have_glite_wms_cp"])

AC_GLITE_WMS_BROKER([], have_glite_wms_br=yes, have_glite_wms_br=no)
AC_MSG_RESULT(["GLITE_WMS_BROKER found $have_glite_wms_br"])

AC_GLITE_WMS_BROKERINFO([], have_glite_wms_brokerinfo=yes, have_glite_wms_brokerinfo=no)
AC_MSG_RESULT(["GLITE_WMS_BROKERINFO found $have_glite_wms_brokerinfo"])

AC_GLITE_WMS_ISM([], have_glite_wms_ism=yes, have_glite_wms_ism=no)
AC_MSG_RESULT(["GLITE_WMS_ISM found $have_glite_wms_ism"])

AC_GLITE_WMS_MATCHMAKING([], have_glite_wms_match=yes, have_glite_wms_match=no)
AC_MSG_RESULT(["GLITE_WMS_MATCHMAKING found $have_glite_wms_match"])

AC_GLITE_SECURITY([], have_glite_security=yes, have_glite_security=no)
AC_MSG_RESULT(["GLITE_SECURITY found $have_glite_security"])

AC_MSG_RESULT(["GLITE_WMS_COMMON found $have_glite_wms_common"])
AC_MSG_RESULT(["GLITE_WMSUTILS found $have_glite_wmsutils"])
AC_GLITE_WMS_OPTIONAL

AC_GLITE_WMS_NS_COMMON([], have_glite_wms_ns_common=yes, have_glite_wms_ns_common=no)
AC_MSG_RESULT(["GLITE_WMS_NS_COMMON found $have_glite_wms_ns_common"])


# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([config/Makefile])
AC_CONFIG_FILES([config/glite-wms-ns])
AC_CONFIG_FILES([src/Makefile])
#AC_CONFIG_FILES([interface/Makefile])

AC_OUTPUT

