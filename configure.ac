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
#       Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
#       Version info: $Id$
#       Release: $Name$

AC_PREREQ(2.57)
AC_INIT([GLite WMS jobsubmission], [3.3.1])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/common/constants.h])

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
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MKTIME
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([ftruncate localtime_r pow strdup strerror strstr])

GLITE_CHECK_LIBDIR
GLITE_CHECK_INITDIR
AC_GLITE_DOCBOOK

AC_BOOST([], AC_MSG_RESULT(["BOOST ok"]), AC_MSG_ERROR(["BOOST not found"]))

PKG_CHECK_MODULES(CLASSAD, emi-classad-ns, , AC_MSG_ERROR(["CLASSADS not found"]))

PKG_CHECK_MODULES(CONDORG, emi-condorg, , AC_MSG_ERROR(["CONDORG not found"]))

condor_version=`pkg-config --modversion emi-condorg`
CONDORG_VERSION=`echo $condor_version | awk -F . '{print $'1'*10000+$'2'*100+$'3'}'`
AC_SUBST(CONDORG_VERSION)
AC_MSG_RESULT([Found condor version: ${CONDORG_VERSION}])

PKG_CHECK_MODULES(GLOBUS_GRAM_PROT, globus-gram-protocol, , AC_MSG_ERROR(["GLOBUS_GRAM_PROT not found"]))

PKG_CHECK_MODULES(GLITE_JOBID, emi-jobid, ,AC_MSG_ERROR(["GLITE_JOBID not found"]))

PKG_CHECK_MODULES(GLITE_WMSUTILS_EXCEPTION, jobman-exception, , AC_MSG_ERROR(["WMSUTILS_EXCEPTION not found"]))
PKG_CHECK_MODULES(GLITE_WMSUTILS_CLASSADS, classad-utils, , AC_MSG_ERROR(["WMSUTILS_CLASSADS not found"]))

PKG_CHECK_MODULES(GLITE_JDL, jdl-api-cpp, , AC_MSG_ERROR(["JDL not found"]))

PKG_CHECK_MODULES(GLITE_WMS_COMMON_CONF, wms-common-conf, , AC_MSG_ERROR(["WMS_COMMON_CONF not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_LDIF2CLASSADS, wms-common-ldif2classads, , AC_MSG_ERROR(["WMS_COMMON_LDIF2CLASSADS not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_LOGGER, wms-common-logger, , AC_MSG_ERROR(["WMS_COMMON_LOGGER not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_PROCESS, wms-common-process, , AC_MSG_ERROR(["WMS_COMMON_PROCESS not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_UT_UTIL, wms-common-util, , AC_MSG_ERROR(["WMS_COMMON_UT_UTIL not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_UT_II, wms-common-ii, , AC_MSG_ERROR(["WMS_COMMON_UT_II not found"]))
PKG_CHECK_MODULES(GLITE_WMS_COMMON_QUOTA, wms-common-quota, , AC_MSG_ERROR(["WMS_COMMON_QUOTA not found"]))

PKG_CHECK_MODULES(GLITE_WMS_PURGER, wms-purger, ,AC_MSG_ERROR(["WMS_PURGER not found"]))

##AC_GLITE_LB_NF([], AC_MSG_RESULT(["GLITE_LB ok"]), AC_MSG_ERROR(["GLITE_LB not found"]))
PKG_CHECK_MODULES(GLITE_LB_COMMON, emi-lb-common, , AC_MSG_ERROR(["GLITE_LB_COMMON not found"]))
PKG_CHECK_MODULES(GLITE_LB_CLIENT, emi-lb-client, , AC_MSG_ERROR(["GLITE_LB_CLIENT not found"]))
PKG_CHECK_MODULES(GLITE_LB_CLIENTPP, emi-lb-clientpp, , AC_MSG_ERROR(["GLITE_LB_CLIENTPP not found"]))

##AC_PROXYRENEWAL([], AC_MSG_RESULT(["PROXY RENEWAL ok"]), AC_MSG_ERROR(["PROXY RENEWAL not found"]))
PKG_CHECK_MODULES(RENEWAL, emi-proxyrenew, , AC_MSG_ERROR(["PROXY RENEWAL not found"]))

PKG_CHECK_MODULES(ARES, libcares, , AC_MSG_ERROR(["ARES not found"]))

# Configuration items
AC_PREFIX_DEFAULT([/usr])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([config/Makefile])
AC_CONFIG_FILES([config/glite-wms-clean-lm-recycle.sh])
AC_CONFIG_FILES([config/glite-wms-jc])
AC_CONFIG_FILES([config/glite-wms-lm])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/common/Makefile])
AC_CONFIG_FILES([src/wms-jss-controller-adapter.pc])
AC_CONFIG_FILES([src/wms-jss-controller-wrapper.pc])
AC_CONFIG_FILES([src/wms-jss-common.pc])
AC_CONFIG_FILES([src/wms-jss-controller.pc])
AC_CONFIG_FILES([src/wms-jss-logmonitor.pc])
AC_CONFIG_FILES([src/controller/Makefile])
AC_CONFIG_FILES([src/logmonitor/Makefile])
AC_CONFIG_FILES([src/logmonitor/processer/Makefile])
AC_CONFIG_FILES([src/daemons/Makefile])
AC_CONFIG_FILES([interface/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([doc/Makefile])

AC_OUTPUT
