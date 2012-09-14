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
#       Revision 1.9  2006/03/20 21:49:39  eronchie
#       Increased version
#
#       Revision 1.8  2005/10/15 11:50:49  eronchie
#       Increased version
#
#       Revision 1.7  2004/12/10 07:44:45  eronchie
#       Increased version
#
#       Revision 1.6  2004/09/22 00:59:39  dimeglio
#       Fixed missing parenthesis in AC_INIT
#
#       Revision 1.5  2004/09/21 19:24:51  glbuild
#       modified module.version
#
#       Revision 1.4  2004/07/28 10:14:40  eronchie
#       Added m4files.list property
#       Removed AC_WMS_COMMON
#
#       Revision 1.3  2004/07/27 11:26:23  eronchie
#       Start using AC_GLITE_WMSUTILS
#
#       Revision 1.2  2004/07/23 17:13:13  eronchie
#       Updatreds
#
#       Revision 1.1  2004/07/08 15:11:03  pandreet
#       Basic build files for checkpointing
#
#
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([GLite WMS checkpointing], [3.1.0])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/jobstate.cpp])

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
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME

# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([memset])

have_boost=no
have_classads=no
                                                                                
AC_BOOST([], have_boost=yes, have_boost=no)
AC_CLASSADS([], have_classads=yes, have_classads=no)
                                                                                
AC_MSG_RESULT(["CLASSADS found $have_classads"])
AC_MSG_RESULT(["BOOST found $have_boost"])

have_glite_wms_common=no
have_glite_wmsutils=no
                                                                                
AC_GLITE

AC_GLITE_WMSUTILS([], have_glite_wmsutils=yes, have_glite_wmsutils=no)
AC_MSG_RESULT(["GLITE_WMSUTILS found $have_glite_wmsutils"])

# Configuration items
AC_PREFIX_DEFAULT([/opt/glite])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([interface/Makefile])
AC_CONFIG_FILES([checkpointing.dox])

AC_OUTPUT

