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
AC_INIT([GLite WMS wmproxy-api-cpp], [3.3.3])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/wmproxy_api.cpp])

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

AH_TEMPLATE([WITH_IPV6], [For Ipv6 support]) 
AC_DEFINE(WITH_IPV6)

# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([memset])

GLITE_CHECK_LIBDIR
AC_GLITE_WMS_WSDL

PKG_CHECK_MODULES(GSOAP, gsoapssl++, , AC_MSG_ERROR(["GSOAP not found"]))
AC_GSOAP_EXTRAS

#AC_GRIDSITE([], AC_MSG_RESULT(["GRIDSITE ok"]), AC_MSG_ERROR(["GRIDSITE not found"]))
PKG_CHECK_MODULES(GRIDSITE_OPENSSL, emi-gridsite-openssl, , AC_MSG_ERROR(["GRIDSITE_OPENSSL not found"]))

DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(OFF)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(glite-wms-wmproxy-api-cpp, project/doxygen/doxygen.cfg, doc)

# Configuration items
AC_PREFIX_DEFAULT([/usr])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/wmproxy-api-cpp.pc])
AC_CONFIG_FILES([interface/Makefile])
AC_CONFIG_FILES([examples/Makefile])

AC_OUTPUT


