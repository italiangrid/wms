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
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS ICE], [3.3.1])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/ice/IceCore.h])

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
AM_PATH_LOG4CPP
#AM_PATH_SQLITE3

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME

# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([memset])

AC_OPTIMIZE
GLITE_CHECK_LIBDIR
GLITE_CHECK_INITDIR

PKG_CHECK_MODULES(ARES, libcares, , AC_MSG_ERROR(["ARES not found"]))

PKG_CHECK_MODULES(GLITE_JOBID, emi-jobid, ,AC_MSG_ERROR(["GLITE_JOBID not found"]))

#PKG_CHECK_MODULES(GLITE_CREAM, cream-client-api-c, , AC_MSG_ERROR(["GLITE_CREAM not found"]))

PKG_CHECK_MODULES(GSOAP, gsoapssl++, , AC_MSG_ERROR(["GSOAP not found"]))
AC_GSOAP_EXTRAS

AC_BOOST([], AC_MSG_RESULT(["BOOST ok"]), AC_MSG_ERROR(["BOOST not found"]))

PKG_CHECK_MODULES(CLASSAD, emi-classad-ns, , AC_MSG_ERROR(["CLASSADS not found"]))

PKG_CHECK_MODULES(GLOBUS_GSI_CRED, globus-gsi-credential, , AC_MSG_ERROR(["GLOBUS_GSI_CRED not found"]))
PKG_CHECK_MODULES(GLOBUS_GSI_PROXY, globus-gsi-proxy-core, , AC_MSG_ERROR(["GLOBUS_GSI_PROXY not found"]))
PKG_CHECK_MODULES(GLOBUS_GSI_UTILS, globus-gsi-cert-utils, , AC_MSG_ERROR(["GLOBUS_GSI_UTILS not found"]))

PKG_CHECK_MODULES(GLITE_WMSUTILS_EXCEPTION, jobman-exception, , AC_MSG_ERROR(["WMSUTILS_EXCEPTION not found"]))
PKG_CHECK_MODULES(GLITE_WMSUTILS_CLASSADS, classad-utils, , AC_MSG_ERROR(["WMSUTILS_CLASSADS not found"]))

PKG_CHECK_MODULES(GLITE_WMS_COMMON_CONF, wms-common-conf, , AC_MSG_ERROR(["WMS_COMMON_CONF not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_LDIF2CLASSADS, wms-common-ldif2classads, , AC_MSG_ERROR(["WMS_COMMON_LDIF2CLASSADS not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_LOGGER, wms-common-logger, , AC_MSG_ERROR(["WMS_COMMON_LOGGER not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_PROCESS, wms-common-process, , AC_MSG_ERROR(["WMS_COMMON_PROCESS not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_UT_UTIL, wms-common-util, , AC_MSG_ERROR(["WMS_COMMON_UT_UTIL not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_UT_II, wms-common-ii, , AC_MSG_ERROR(["WMS_COMMON_UT_II not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_QUOTA, wms-common-quota, , AC_MSG_ERROR(["WMS_COMMON_QUOTA not found"]))

PKG_CHECK_MODULES(GLITE_WMS_PURGER, wms-purger, ,AC_MSG_ERROR(["WMS_PURGER not found"]))

PKG_CHECK_MODULES(GLITE_LB_COMMON, emi-lb-common, , AC_MSG_ERROR(["GLITE_LB_COMMON not found"]))
PKG_CHECK_MODULES(GLITE_LB_CLIENT, emi-lb-client, , AC_MSG_ERROR(["GLITE_LB_CLIENT not found"]))
PKG_CHECK_MODULES(GLITE_LB_CLIENTPP, emi-lb-clientpp, , AC_MSG_ERROR(["GLITE_LB_CLIENTPP not found"]))

PKG_CHECK_MODULES(RENEWAL, emi-proxyrenew, , AC_MSG_ERROR(["PROXY RENEWAL not found"]))

PKG_CHECK_MODULES(GSOAP_PLUGIN, emi-gsoap-plugin, , AC_MSG_ERROR(["GSOAP_PLUGIN not found"]))

PKG_CHECK_MODULES(GRIDSITE_OPENSSL, emi-gridsite-openssl, , AC_MSG_ERROR(["GRIDSITE not found"]))

PKG_CHECK_MODULES(VOMS, voms-2.0, , AC_MSG_ERROR(["VOMS not found"]))

PKG_CHECK_MODULES(MYPROXY, myproxy, , AC_MSG_ERROR(["MYPROXY not found"]))

AC_PREFIX_DEFAULT([/usr])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([config/Makefile])
AC_CONFIG_FILES([config/glite-wms-ice])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/iceDb/Makefile])
AC_CONFIG_FILES([src/iceDb/sqlite/Makefile])
AC_CONFIG_FILES([src/iceUtils/Makefile])
AC_CONFIG_FILES([src/ice/Makefile])
AC_CONFIG_FILES([src/iceCommands/Makefile])
AC_CONFIG_FILES([src/iceThreads/Makefile])
EXPORT_DYNAMIC_LDFLAGS=-rdynamic
AC_SUBST([EXPORT_DYNAMIC_LDFLAGS])

AC_OUTPUT
