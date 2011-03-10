## Copyright (c) Members of the EGEE Collaboration. 2009. 
## See http://www.eu-egee.org/partners/ for details on the copyright holders.  

## Licensed under the Apache License, Version 2.0 (the "License"); 
## you may not use this file except in compliance with the License. 
## You may obtain a copy of the License at 
##     http://www.apache.org/licenses/LICENSE-2.0 
## Unless required by applicable law or agreed to in writing, software 
## distributed under the License is distributed on an "AS IS" BASIS, 
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
## See the License for the specific language governing permissions and 
## limitations under the License.

#       Common configure.ac file for the GLite WMS Common module
#
#       Authors: Alberto Di Meglio <alberto.di.meglio@cern.ch>
#       Version info: $Id$
#       Release: $Name$
#
#       Revision history:
#       $Log$
#       Revision 1.16.2.18.4.6  2011/03/10 15:25:59  pandreet
#       Removed with-glite-location
#
#       Revision 1.16.2.18.4.5  2011/03/07 09:57:11  pandreet
#       Incremented version number
#
#       Revision 1.16.2.18.4.4  2011/01/05 08:49:55  pandreet
#       Pkg-config support for purger
#
#       Revision 1.16.2.18.4.3  2010/09/03 08:03:33  pandreet
#       Ares AC definitions in build files
#
#       Revision 1.16.2.18.4.2  2010/04/08 13:59:12  mcecchi
#       copyright and license
#
#       Revision 1.16.2.18.4.1  2009/06/08 14:46:33  fcapanni
#       porting to org.glite.jobid
#
#       Revision 1.16.2.18  2008/07/28 14:15:37  monforte
#       Increased module version number
#
#       Revision 1.16.2.17  2008/05/20 11:00:33  monforte
#       increased version
#
#       Revision 1.16.2.16  2008/05/14 08:47:08  monforte
#       Fix for:
#       - bug #36536: The glite wms purge storage library should rely on LBProxy while logging CLEAR events.
#       - bug #35156: glite-wms-purgeStorage.sh hardcodes proxy file name
#
#       Revision 1.16.2.15  2007/11/08 14:24:03  giaco
#       increased version
#
#       Revision 1.16.2.14  2007/11/08 11:22:04  giaco
#       clean up expat and jp deps
#
#       Revision 1.16.2.13  2007/11/06 11:21:20  monforte
#       Increased version numbers
#
#       Revision 1.16.2.12  2007/11/06 09:52:34  monforte
#       Removed libtar and jp dependencies + some minor cleanup
#
#       Revision 1.16.2.11  2007/06/05 09:32:02  rlops
#       decreased version
#
#       Revision 1.16.2.10  2007/05/31 12:15:30  rlops
#       *** empty log message ***
#
#       Revision 1.16.2.9  2007/05/31 11:43:08  eronchie
#       Added new script
#
#       Revision 1.16.2.8  2007/05/25 11:38:09  monforte
#       Addressing bug #26705
#
#       Revision 1.16.2.7  2007/05/10 14:43:48  monforte
#       Fix to bug #26267: The purger does not work: creation of lb context always fails.
#
#       Revision 1.16.2.6  2007/05/09 09:21:14  rlops
#       increased version
#
#       Revision 1.16.2.5  2007/04/24 08:50:36  rlops
#       increased version
#
#       Revision 1.16.2.4  2007/02/21 15:56:44  rlops
#       increased version
#
#       Revision 1.16.2.3  2006/11/21 16:25:10  eronchie
#       Increased version
#
#       Revision 1.16.2.2  2006/08/29 11:24:36  monforte
#       Added printing of skipping reason
#
#       Revision 1.16.2.1  2006/04/03 11:49:53  eronchie
#       Increased version
#       Fix problem in the tag 3.1.0
#
#       Revision 1.16  2006/03/20 22:52:54  eronchie
#       Increased version
#
#       Revision 1.15  2006/03/17 13:20:43  monforte
#       Removing ares inclusion.
#
#       Revision 1.14  2006/03/14 11:51:36  monforte
#       *** empty log message ***
#
#       Revision 1.13  2006/03/07 11:39:01  monforte
#       Merge against glite-wms-purger_branch_1_5_0_RC15
#
#       Revision 1.12.2.10  2006/01/20 00:27:21  eronchie
#       Increased version
#
#       Revision 1.12.2.9  2005/12/06 18:56:07  monforte
#       Increased version number
#
#       Revision 1.12.2.8  2005/12/06 15:53:22  monforte
#       Added handling of --enable-jp-integration configuration flag to toggle jp code
#       integration
#
#       Revision 1.12.2.7  2005/12/02 10:51:08  eronchie
#       Increased version
#
#       Revision 1.12.2.6  2005/12/01 19:16:09  eronchie
#       Correct version according to the latest tag
#
#       Revision 1.12.2.5  2005/12/01 15:36:42  monforte
#       Disabled JP Api calls and library linking
#
#       Revision 1.12.2.4  2005/11/17 08:36:43  monforte
#       LBProxy + JP + Fix
#
#       Revision 1.12.2.3  2005/10/20 16:20:59  eronchie
#       Fix a problem
#
#       Revision 1.12.2.2  2005/10/19 11:02:26  eronchie
#       Added macros to correctly use the jp api
#
#       Revision 1.12.2.1  2005/10/19 10:08:42  eronchie
#       Increased version
#
#       Revision 1.12  2005/10/15 12:18:39  eronchie
#       Increased version
#
#       Revision 1.11  2005/09/19 11:00:08  monforte
#       Merge against  glite-wms-purger_branch_1_0_2
#
#       Revision 1.10.2.4.2.5  2005/08/17 14:20:33  eronchie
#       Increased version
#
#       Revision 1.10.2.4.2.4  2005/08/17 14:19:43  eronchie
#       Added ERROR message that breaks configure
#
#       Revision 1.10.2.4.2.3  2005/08/03 14:21:47  monforte
#       Added hard-coded sequence code
#
#       Revision 1.10.2.4.2.2  2005/07/07 11:32:01  fprelz
#       Increased version: removed ambiguous method.
#
#       Revision 1.10.2.4.2.1  2005/06/30 10:57:30  monforte
#       Fixed a problem in the logic of purgeStorage function return value
#
#       Revision 1.10.2.4  2005/05/30 11:21:29  monforte
#       Merge against glite-wms-purger_branch_1_0_0_RC1_WMProxy
#
#       Revision 1.10.4.1  2005/03/18 11:20:44  monforte
#       Fixed purging of DAG
#
#       Revision 1.10  2004/12/10 07:52:20  eronchie
#       Increased version
#
#       Revision 1.9  2004/09/22 01:09:09  dimeglio
#       Fixed missing parenthesis in AC_INIT
#
#       Revision 1.8  2004/09/21 19:25:04  glbuild
#       modified module.version
#
#       Revision 1.7  2004/07/28 11:49:15  eronchie
#       Updated package name
#
#       Revision 1.6  2004/07/28 08:10:02  eronchie
#       Updated
#
#       Revision 1.5  2004/07/28 06:59:57  eronchie
#       Removed interface
#
#       Revision 1.4  2004/07/27 11:42:00  eronchie
#       Start using AC_GLITE_WMSUTILS
#
#       Revision 1.3  2004/07/25 18:25:37  eronchie
#       Start adding purger
#       WARNING: work in progress
#
#       Revision 1.2  2004/07/25 18:05:23  eronchie
#       Updated according to the new components
#
#       Revision 1.1.1.1  2004/07/18 12:14:57  eronchie
#       Start adding purger component
#
#       Revision 1.7  2004/06/21 12:56:33  eronchie
#       Start handling of interface headers
#
#       Revision 1.6  2004/06/16 15:57:56  eronchie
#       Added configure options
#       WARNING: work in progress
#
#       Revision 1.5  2004/06/16 08:24:07  eronchie
#       Added configure options
#       WARNING: work in progress
#
#       Revision 1.4  2004/06/15 17:35:14  eronchie
#       Added configure options
#       WARNING: work in progress
#
#       Revision 1.3  2004/06/14 17:50:09  eronchie
#       Added configure options
#       WARNING Work in progress
#
#       Revision 1.2  2004/05/31 13:39:51  eronchie
#       Merged with code in the infnforge repository
#
#       Revision 1.1.1.2  2004/05/26 18:24:28  eronchie
#       Import of wms tls
#
#
                                                                                                                                                             
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS Purger], [3.3.1])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/purger.cpp])

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

have_glite_wms_common=no
have_glite_wmsutils=no
have_glite_lb=no
have_glite_jobid=no

GLITE_CHECK_LIBDIR
AC_OPTIMIZE
AC_ARES

AC_GLITE_WMS_COMMON([], have_glite_wms_common=yes, have_glite_wms_common=no)
AC_MSG_RESULT(["GLITE_WMS_COMMON found $have_glite_wms_common"])

AC_GLITE_WMSUTILS([], have_glite_wmsutils=yes, have_glite_wmsutils=no)
AC_MSG_RESULT(["GLITE_WMSUTILS found $have_glite_wmsutils"])

AC_GLITE_LB([], have_glite_lb=yes, have_glite_lb=no)
AC_MSG_RESULT(["GLITE_LB found $have_glite_lb"])

GLITE_CPPFLAGS=${GLITE_CFLAGS}
GLITE_LDFLAGS="${GLITE_LDFLAGS} -L$GLITE_LOCATION/lib"
GLITE_CHECK_JOBID([], have_glite_jobid=yes, have_glite_jobid=no)
AC_MSG_RESULT(["GLITE_JOBID found $have_glite_jobid"])

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/wms-purger.pc])
AC_CONFIG_FILES([config/Makefile])

AC_OUTPUT

