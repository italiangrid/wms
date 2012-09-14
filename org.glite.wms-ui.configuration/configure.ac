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
#       Revision 1.9  2009/01/20 11:00:53  lpetronz
#       SL5 build fixing: not needed marcos removed
#
#       Revision 1.8  2006/03/20 15:45:23  eronchie
#       Increased version
#
#       Revision 1.7  2004/12/17 13:53:13  amarasch
#       Version updated
#
#       Revision 1.6  2004/12/10 09:47:53  amarasch
#       Increased version
#
#       Revision 1.5  2004/10/19 12:00:24  eronchie
#       Removed config.h handling
#
#       Revision 1.4  2004/10/15 15:52:23  eronchie
#       Start adding ui configuration
#
#       Revision 1.3  2004/10/13 15:34:31  eronchie
#       ......
#
#       Revision 1.2  2004/10/13 15:11:30  eronchie
#       Updated
#
#       Revision 1.1  2004/10/13 14:57:05  eronchie
#       Start adding configuration
#
#
#
                                                                                                                                                             
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS UI Configuration], [3.1.0])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([config/glite_wmsui.conf])

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

AC_GLITE

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
dnl AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([config/Makefile])
AC_CONFIG_FILES([config/glite-wmsui-env.csh])
AC_CONFIG_FILES([config/glite-wmsui-env.sh])
AC_CONFIG_FILES([config/glite-wmsui-gui-env.csh])
AC_CONFIG_FILES([config/glite-wmsui-gui-env.sh])
AC_CONFIG_FILES([config/glite-wmsui.csh])
AC_CONFIG_FILES([config/glite-wmsui.sh])
AC_CONFIG_FILES([config/glite-wmsui-vars.csh])
AC_CONFIG_FILES([config/glite-wmsui-vars.sh])

AC_OUTPUT

