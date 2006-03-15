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
#
#       Revision history:
#       $Log$
#       Revision 1.19  2006/03/15 12:55:51  gianelle
#       Added (macro) support for:
#       - libtar
#       - Optimization
#       - Gpbox
#       - JP
#       - Purger
#
#       Revision 1.18  2006/03/14 21:22:14  eronchie
#       Applied PURGER macro
#
#       Revision 1.17  2005/10/15 12:03:41  eronchie
#       Increased version
#
#       Revision 1.16  2005/09/20 14:11:36  gianelle
#       Syncronize the rc1 branch with head.
#
#       Revision 1.15  2004/12/10 07:50:30  eronchie
#       Increased version
#
#       Revision 1.14  2004/11/01 10:39:52  dimeglio
#       Incremented version number
#
#       Revision 1.13  2004/09/22 01:04:17  dimeglio
#       Fixed missing parenthesis in AC_INIT
#
#       Revision 1.12  2004/09/21 19:25:48  glbuild
#       modified module.version
#
#       Revision 1.11  2004/08/27 13:19:40  eronchie
#       Added handling of config directory
#       Added start up script files
#
#       Revision 1.10  2004/07/30 08:17:38  eronchie
#       Increased condor version 6.7.1
#
#       Revision 1.9  2004/07/28 14:38:57  pandreet
#       Proxyrenewal dependencies fixed
#
#       Revision 1.8  2004/07/28 10:19:09  eronchie
#       Added m4files.list property
#
#       Revision 1.7  2004/07/27 13:46:37  pandreet
#       LB dependencies fixed
#
#       Revision 1.6  2004/07/27 11:39:01  eronchie
#       *** empty log message ***
#
#       Revision 1.5  2004/07/27 09:41:25  pandreet
#       JobId and Exception dependencies fixed
#
#       Revision 1.4  2004/07/25 16:42:14  eronchie
#       Updated jobid, and tls
#       Moved out from org.glite.wms and put in org.glite.wms-utils
#
#       Revision 1.3  2004/07/18 11:56:40  eronchie
#       Updated LB dependency
#
#       Revision 1.2  2004/07/18 11:53:35  eronchie
#       Added AC_GLITE_LB check
#       WARNING: work in progress
#
#       Revision 1.1  2004/07/06 10:05:38  pandreet
#       Basic build files for jobsubmission
#
#
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS jobsubmission], [1.5.0])
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

have_boost=no
have_classads=no
have_condorg=no
have_globus=no
have_ares=no
have_expat=no
have_libtar=no
                                                                                
AC_BOOST([], have_boost=yes, have_boost=no)
AC_CLASSADS([], have_classads=yes, have_classads=no)
AC_CONDORG(6.7.1, have_condorg=yes, have_condorg=no)
AC_GLOBUS([], have_globus=yes, have_globus=no)
AC_ARES([], have_ares=yes, have_ares=no)
AC_EXPAT([], have_expat=yes, have_expat=no)
AC_LIBTAR([], have_libtar=yes, have_libtar=no)

AC_MSG_RESULT(["CLASSADS found $have_classads"])
AC_MSG_RESULT(["BOOST found $have_boost"])
AC_MSG_RESULT(["CONDORG found $have_condorg"])
AC_MSG_RESULT(["GLOBUS found $have_globus"])
AC_MSG_RESULT(["ARES found $have_ares"])
AC_MSG_RESULT(["EXPAT found $have_expat"])
AC_MSG_RESULT(["LIBTAR found $have_libtar"])

have_glite_wms_common=no
have_glite_wmsutils=no
have_glite_wms_jdl=no
have_glite_wms_purger=no
have_glite_lb=no
have_glite_security=no                                                       
have_glite_jp=no
                      
AC_GLITE
                                                                                
AC_GLITE_WMS_COMMON([], have_glite_wms_common=yes, have_glite_wms_common=no)
AC_MSG_RESULT(["GLITE_WMS_COMMON found $have_glite_wms_common"])

AC_GLITE_WMSUTILS([], have_glite_wmsutils=yes, have_glite_wmsutils=no)
AC_MSG_RESULT(["GLITE_WMSUTILS found $have_glite_wmsutils"])

AC_GLITE_WMS_JDL([], have_glite_wms_jdl=yes, have_glite_wms_jdl=no)
AC_MSG_RESULT(["GLITE_WMS_JDL found $have_glite_wms_jdl"])

AC_GLITE_WMS_PURGER([], have_glite_wms_purger=yes, have_glite_wms_purger=no)
AC_MSG_RESULT(["GLITE_WMS_PURGER found $have_glite_wms_purger"])

AC_GLITE_LB([], have_glite_lb=yes, have_glite_lb=no)
AC_MSG_RESULT(["GLITE_LB found $have_glite_lb"])

AC_GLITE_JP([], have_glite_jp=yes, have_glite_jp=no)
AC_MSG_RESULT(["GLITE_JP found $have_glite_jp"])

AC_GLITE_SECURITY([], have_glite_security=yes, have_glite_security=no)
AC_MSG_RESULT(["GLITE_SECURITY found $have_glite_security"])

AC_GLITE_GPBOX
AC_GLITE_WMS_OPTIONAL

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([config/Makefile])
AC_CONFIG_FILES([config/glite-wms-clean-lm-recycle.sh])
AC_CONFIG_FILES([config/glite-wms-jc])
AC_CONFIG_FILES([config/glite-wms-lm])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/common/Makefile])
AC_CONFIG_FILES([src/controller/Makefile])
AC_CONFIG_FILES([src/logmonitor/Makefile])
AC_CONFIG_FILES([src/logmonitor/processer/Makefile])
AC_CONFIG_FILES([src/daemons/Makefile])
AC_CONFIG_FILES([test/Makefile])

AC_OUTPUT

