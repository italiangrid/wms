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
#       Authors: Giuseppe Avellino <giuseppe.avellino@datamat.it>
#

AC_PREREQ(2.57)
AC_INIT([GLite WMS client], [3.1.0])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/commands/wmp-job-submit.cpp])
AC_CONFIG_SRCDIR([config/glite_wmsclient.conf])

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
AC_PROG_YACC
AM_PROG_LEX
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

have_boost=no
have_classads=no
have_globus=no
have_gridsite=no
have_expat=no
have_gridsite=no
have_libtar=no
have_zlib=no

AC_BOOST([], have_boost=yes, have_boost=no)
AC_MSG_RESULT(["BOOST found $have_boost"])

AC_CLASSADS([], have_classads=yes, have_classads=no)
AC_MSG_RESULT(["CLASSADS found $have_classads"])


AC_EXPAT([], have_expat=yes, have_expat=no)
AC_MSG_RESULT(["EXPAT found $have_expat"])

AC_GLOBUS([], have_globus=yes, have_globus=no)
AC_MSG_RESULT(["GLOBUS found $have_globus"])

AC_GRIDSITE([], have_gridsite=yes, have_gridsite=no)
AC_MSG_RESULT(["GRIDSITE found $have_gridsite"])

AC_LIBTAR([], have_libtar=yes, have_libtar=no)
AC_MSG_RESULT(["LIBTAR found $have_libtar"])

AC_ZLIB([], have_zlib=yes, have_zlib=no)
AC_MSG_RESULT(["ZLIB found $have_zlib"])

AC_GSOAP

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

if test "$have_gridsite" = "no"; then
  AC_MSG_RESULT(["Please verify GRIDSITE package"])
fi

if test "$have_libtar" = "no"; then
  AC_MSG_RESULT(["Please verify LIBTAR package"])
fi

if test "$have_zlib" = "no"; then
  AC_MSG_RESULT(["Please verify ZLIB package"])
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

if test "$have_gridsite" = "no"; then
  AC_MSG_ERROR([Cannot continue building])
fi

if test "$have_libtar" = "no"; then
  AC_MSG_ERROR([Cannot continue building])
fi

if test "$have_zlib" = "no"; then
  AC_MSG_ERROR([Cannot continue building])
fi

have_glite_wmsutils=no
have_glite_jdl=no
have_glite_lb=no
have_glite_wms_chkpt=no
have_glite_wms_wmproxy=no
have_glite_security=no

AC_GLITE
AC_GLITE_SERVICE_DISCOVERY
AC_OPTIMIZE


AC_GLITE_WMSUTILS([], have_glite_wmsutils=yes, have_glite_wmsutils=no)
AC_MSG_RESULT(["GLITE_WMSUTILS found $have_glite_wmsutils"])

AC_GLITE_JDL([], have_glite_jdl=yes, have_glite_jdl=no)
AC_MSG_RESULT(["GLITE_JDL found $have_glite_jdl"])

AC_GLITE_LB([], have_glite_lb=yes, have_glite_lb=no)
AC_MSG_RESULT(["GLITE_LB found $have_glite_lb"])

AC_GLITE_WMS_CHKPT([], have_glite_wms_chkpt=yes, have_glite_wms_chkpt=no)
AC_MSG_RESULT(["GLITE_WMS_CHKPT found $have_glite_wms_chkpt"])

AC_GLITE_WMS_WMPROXY([], have_glite_wms_wmproxy=yes, have_glite_wms_wmproxy=no)
AC_MSG_RESULT(["GLITE_WMS_WMPROXY found $have_glite_wms_wmproxy"])

AC_GLITE_SECURITY([], have_glite_security=yes, have_glite_security=no)
AC_MSG_RESULT(["GLITE_SECURITY found $have_glite_security"])

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([config/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/utilities/Makefile])
AC_CONFIG_FILES([src/services/Makefile])
AC_CONFIG_FILES([src/lbclient/Makefile])
AC_CONFIG_FILES([src/commands/Makefile])
AC_CONFIG_FILES([doc/Makefile])
AC_CONFIG_FILES([wms-client.dox])
AC_CONFIG_FILES([config/glite-wmsclient.csh])
AC_CONFIG_FILES([config/glite-wmsclient.sh])
AC_CONFIG_FILES([config/glite-wmsclient-vars.csh])
AC_CONFIG_FILES([config/glite-wmsclient-vars.sh])


AC_OUTPUT


