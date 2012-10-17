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

AC_PREREQ(2.57)
AC_INIT([GLite WMS Common], [3.3.1])
AC_CONFIG_AUX_DIR([./project])
AM_INIT_AUTOMAKE([1.6.3 subdir-objects])
AC_CONFIG_SRCDIR([src/ldif2classad/LDAPForwardIterator.h])

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

GLITE_CHECK_LIBDIR
AC_OPTIMIZE
GLITE_CHECK_LDAP

AC_BOOST([], AC_MSG_RESULT(["BOOST ok"]), AC_MSG_ERROR(["BOOST not found"]))

PKG_CHECK_MODULES(CLASSAD, emi-classad-ns, , AC_MSG_ERROR(["CLASSADS not found"]))

PKG_CHECK_MODULES(GLOBUS_COMMON, globus-common, , AC_MSG_ERROR(["GLOBUS_COMMON not found"]))

PKG_CHECK_MODULES(CPPUNIT, cppunit, , AC_MSG_ERROR(["CPPUNIT not found"]))

PKG_CHECK_MODULES(GLITE_WMSUTILS_EXCEPTION, jobman-exception, , AC_MSG_ERROR(["WMSUTILS_EXCEPTION not found"]))
PKG_CHECK_MODULES(GLITE_WMSUTILS_CLASSADS, classad-utils, , AC_MSG_ERROR(["WMSUTILS_CLASSADS not found"]))

PKG_CHECK_MODULES(GLITE_JOBID, emi-jobid, ,AC_MSG_ERROR(["GLITE_JOBID not found"]))

# Configuration items
AC_PREFIX_DEFAULT([/usr])
AM_CONFIG_HEADER([src/autogen/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([interface/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/wms-common.pc])
AC_CONFIG_FILES([src/wms-common-conf.pc])
AC_CONFIG_FILES([src/wms-common-ldif2classads.pc])
AC_CONFIG_FILES([src/wms-common-process.pc])
AC_CONFIG_FILES([src/wms-common-util.pc])
AC_CONFIG_FILES([src/wms-common-ii.pc])
AC_CONFIG_FILES([src/wms-common-logger.pc])        
AC_CONFIG_FILES([src/wms-common-quota.pc])
AC_CONFIG_FILES([src/scripts/Makefile])
AC_CONFIG_FILES([src/configuration/Makefile])
AC_CONFIG_FILES([src/ldif2classad/Makefile])
AC_CONFIG_FILES([src/logger/Makefile])
AC_CONFIG_FILES([src/utilities/Makefile])
AC_CONFIG_FILES([src/process/Makefile])
AC_CONFIG_FILES([src/programs/Makefile])
AC_CONFIG_FILES([test/Makefile])

AC_OUTPUT

