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
#       Revision 1.20  2007/07/24 09:22:12  acava
#       bug #28178: NS dependencies still inside CLI-Python and Wrap-Python
#
#       Revision 1.19  2007/04/27 16:19:40  lpetronz
#       Code Restructuring: removed checkpointing and partitioner dependency
#
#       Revision 1.18  2007/04/06 10:15:14  amarasch
#       Merged From 3.1 (configuration files)
#
#       Revision 1.16.2.16  2007/04/03 15:27:51  amarasch
#       Minor version upgraded due to important changes: LB automatic generation
#
#       Revision 1.16.2.15  2007/04/03 08:06:50  rlops
#       increased version
#
#       Revision 1.16.2.14  2007/02/21 17:50:38  amarasch
#       AC PYTHON set to 2.2.3
#
#       Revision 1.16.2.13  2007/02/21 17:42:24  amarasch
#       version updated to 3.1.9 (python version for SL4
#
#       Revision 1.16.2.12  2007/02/21 17:40:02  amarasch
#       ETICS python version updated
#
#       Revision 1.16.2.11  2007/02/20 13:55:34  rlops
#       increased version
#
#       Revision 1.16.2.10  2007/02/08 16:18:25  rlops
#       increased version
#
#       Revision 1.16.2.9  2007/01/25 13:47:04  amarasch
#       ns m4 added to compilation (yet still not working, libraries specified by hand)
#
#       Revision 1.16.2.8  2007/01/17 16:12:49  rlops
#       added a flag to produce what ui is needed when ns build is disabled
#
#       Revision 1.16.2.7  2006/12/06 17:40:42  eronchie
#       Increased version
#
#       Revision 1.16.2.6  2006/11/22 11:28:15  eronchie
#       Increased version
#
#       Revision 1.16.2.5  2006/09/22 10:00:55  amarasch
#       version updated for new tag 3.1.4
#
#       Revision 1.16.2.4  2006/08/01 10:08:06  amarasch
#       Version 3.1.3: voms parsed correctly
#
#       Revision 1.16.2.3  2006/07/26 16:16:29  amarasch
#       Service Discovery library properly linked
#
#       Revision 1.16.2.2  2006/06/27 16:08:40  amarasch
#       bug #17601 fix: WMS RPMS should not contain Change me for i in org.glite.wms-ui.api-cpp  org.glite.wms-ui.api-java org.glite.wms-ui.cli-python org.glite.wms-ui.configuration org.glite.wms-ui.gui-java org.glite.wms-ui.wrap-java org.glite.wms-ui.wrap-python ; do echo ********; echo org.glite.wms-ui.wrap-python ; echo ********* ; \! in summary / description
#
#       Revision 1.16.2.1  2006/04/19 15:15:25  amarasch
#       version updated for incoming new tag (bugfix #16029)
#
#       Revision 1.16  2006/03/20 15:52:38  eronchie
#       *** empty log message ***
#
#       Revision 1.15  2006/03/15 20:48:05  eronchie
#       Updated according to the new location of jdl library and classad_utils library
#
#       Revision 1.14  2006/03/15 10:31:12  amarasch
#       Ares migration to c-ares
#
#       Revision 1.13  2005/09/22 08:58:17  amarasch
#       Merged from RC1 branch
#
#
#       Revision 1.10  2004/12/10 09:49:01  amarasch
#       Increased version
#
#       Revision 1.9  2004/12/09 15:05:21  eronchie
#       Updated
#
#       Revision 1.8  2004/10/11 10:39:18  fpacini
#       added dependencies
#
#       Revision 1.7  2004/09/22 01:35:52  dimeglio
#       Fixed missing parenthesis in AC_INIT
#
#       Revision 1.6  2004/09/21 19:26:24  glbuild
#       modified module.version
#
#       Revision 1.5  2004/08/12 16:04:55  eronchie
#       Updated
#
#       Revision 1.4  2004/08/12 09:26:33  eronchie
#       Updated
#
#       Revision 1.3  2004/08/12 08:16:32  eronchie
#       Updated
#
#       Revision 1.2  2004/08/11 16:00:53  eronchie
#       Updated
#
#       Revision 1.1  2004/08/11 14:52:43  eronchie
#       Start adding wrap-python
#
#

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS UI Wrap python], [3.2.0])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/NsWrapper.h])

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
have_expat=no
have_cppunit=no
have_swig=no
have_python=no

AC_BOOST([], have_boost=yes, have_boost=no)
AC_CLASSADS([], have_classads=yes, have_classads=no)
AC_GLOBUS([], have_globus=yes, have_globus=no)
AC_CPPUNIT([], have_cppunit=yes, have_cppunit=no)
AC_EXPAT([], have_expat=yes, have_expat=no)
AC_SWIG([1.3.21], have_swig=yes, have_swig=no)
AC_PYTHON([2.2.3], have_python=yes, have_python=no)

AC_MSG_RESULT(["CLASSADS found $have_classads"])
AC_MSG_RESULT(["BOOST found $have_boost"])
AC_MSG_RESULT(["GLOBUS found $have_globus"])
AC_MSG_RESULT(["CPPUNIT found $have_cppunit"])
AC_MSG_RESULT(["EXPAT found $have_expat"])
AC_MSG_RESULT(["SWIG found $have_swig"])
AC_MSG_RESULT(["PYTHON found $have_python"])

have_glite_wms_common=no
have_glite_wmsutils=no
have_glite_jdl=no
have_glite_wms_part=no
have_lb=no
have_dgas=no
have_glite_dgas_common=no
have_glite_security=no

AC_GLITE

AC_GLITE_SERVICE_DISCOVERY


AC_GLITE_WMS_NS_CLIENT([], have_glite_wms_ns_client=yes, have_glite_wms_ns_client=no)
AC_MSG_RESULT(["GLITE_WMS_NS_CLIENT found $have_glite_wms_ns_client"])

AC_GLITE_WMS_NS_COMMON([], have_glite_wms_ns_common=yes, have_glite_wms_ns_common=no)
AC_MSG_RESULT(["GLITE_WMS_NS_COMMON found $have_glite_wms_ns_common"])

AC_GLITE_WMS_COMMON([], have_glite_wms_common=yes, have_glite_wms_common=no)
AC_MSG_RESULT(["GLITE_WMS_COMMON found $have_glite_wms_common"])

AC_GLITE_WMSUTILS([], have_glite_wmsutils=yes, have_glite_wmsutils=no)
AC_MSG_RESULT(["GLITE_WMSUTILS found $have_glite_wmsutils"])

AC_GLITE_JDL([], have_glite_jdl=yes, have_glite_jdl=no)
AC_MSG_RESULT(["GLITE_JDL found $have_glite_jdl"])

AC_GLITE_LB([], have_glite_lb=yes, have_glite_lb=no)
AC_MSG_RESULT(["GLITE_LB found $have_glite_lb"])

AC_GLITE_DGAS([], have_glite_dgas=yes, have_glite_dgas=no)
AC_MSG_RESULT(["GLITE_DGAS found $have_glite_dgas"])

AC_GLITE_DGAS_COMMON([], have_glite_dgas_common=yes, have_glite_dgas_common=no)
AC_MSG_RESULT(["GLITE_DGAS_COMMON found $have_glite_dgas_common"])

AC_VOMS([], have_voms=yes, have_voms=no)
AC_MSG_RESULT(["VOMS found $have_voms"])

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])

AC_OUTPUT

