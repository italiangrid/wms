project(wms-server C CXX)
cmake_minimum_required(VERSION 2.6)

if(NOT ${CMAKE_C_COMPILER} STREQUAL "CMAKE_C_COMPILER-NOTFOUND")
  set(OK_C_COMPILER "1")
endif(NOT ${CMAKE_C_COMPILER} STREQUAL "CMAKE_C_COMPILER-NOTFOUND")

if(NOT ${CMAKE_CXX_COMPILER} STREQUAL "CMAKE_CXX_COMPILER-NOTFOUND")
  set(OK_CXX_COMPILER "1")
endif(NOT ${CMAKE_CXX_COMPILER} STREQUAL "CMAKE_CXX_COMPILER-NOTFOUND")


EXEC_PROGRAM(/bin/date ARGS +'%a %b %d %Y' OUTPUT_VARIABLE DATE)

if(NOT DEFINED OS)
    message(FATAL_ERROR " Undefined Operating System name. Please use -DOS=<os_name>
to customize (choices are \"sl5\", \"sl6\" )")
endif(NOT DEFINED OS)

if(NOT DEFINED MOCKCFG)
    message(WARNING " Undefined mock configuration name. Please use
-DMOCKCFG=<name> for building in mock. Setting to 'none'")
endif(NOT DEFINED MOCKCFG)

if(NOT DEFINED AGE)
    message(WARNING " Undefined age number, selected 0 Please use -DAGE=<number> to customize")
    set(AGE "0")
endif(NOT DEFINED AGE)

if(NOT DEFINED PVER)
    message(WARNING " Undefined version number, selected 0.0.0. Please use
-DPVER=<version> to customize")
    set(PVER "0.0.0")
endif(NOT DEFINED PVER)

if(NOT DEFINED PREFIX)
    message(WARNING " Undefined prefix, selected /usr/local")
    set(PREFIX "/tmp/")
endif(NOT DEFINED PREFIX)

if(NOT DEFINED PKG_INSTALL_PREFIX)
    set(PKG_INSTALL_PREFIX "${PREFIX}")
endif(NOT DEFINED PKG_INSTALL_PREFIX)

if( ${CMAKE_VERSION} VERSION_LESS "2.8.2" )
    if( IS_DIRECTORY "/usr/lib64" )
        set(LIBDIR "lib64")
    else( IS_DIRECTORY "/usr/lib64" )
        set(LIBDIR "lib")
    endif( IS_DIRECTORY "/usr/lib64" )
else( ${CMAKE_VERSION} VERSION_LESS "2.8.2" )
    if( IS_DIRECTORY "/usr/lib64" AND NOT IS_SYMLINK "/usr/lib64" )
        set(LIBDIR "lib64")
    else( IS_DIRECTORY "/usr/lib64" AND NOT IS_SYMLINK "/usr/lib64" )
        set(LIBDIR "lib")
    endif( IS_DIRECTORY "/usr/lib64" AND NOT IS_SYMLINK "/usr/lib64" )
endif( ${CMAKE_VERSION} VERSION_LESS "2.8.2" )

#message(FATAL " ${PROJECT_SOURCE_DIR}" )

IF (UNIX)
  # since it's unix-specific we will use bash
  ADD_CUSTOM_TARGET (distclean @echo cleaning ${FASTLIB_SOURCE_DIR}
for source distribution)
  ADD_CUSTOM_TARGET (configuration @echo building only configuration)
  ADD_CUSTOM_TARGET (common @echo building only common)
  ADD_CUSTOM_TARGET (purger @echo building only purger)
  ADD_CUSTOM_TARGET (core @echo building only core)
  ADD_CUSTOM_TARGET (interface @echo building only interface)
  ADD_CUSTOM_TARGET (jobsubmission @echo building only jobsubmission)
  ADD_CUSTOM_TARGET (ice @echo building only ice)
  ADD_CUSTOM_TARGET (RPM @echo packaging RPMS and SRPMS)
  ADD_CUSTOM_TARGET (MOCK @echo trying to build in the mock environment)

  ADD_CUSTOM_COMMAND(TARGET MOCK
    COMMAND mock ARGS -r ${MOCKCFG} rpmbuild/SRPMS/glite-wms-server-${PVER}-${AGE}.${OS}.src.rpm
  )
  
  ADD_CUSTOM_COMMAND(TARGET RPM
    # this will create the stage area needed by rpm packager (just in case if
    # the uses didn't it already
    COMMAND make ARGS install
    COMMAND make ARGS nagios
    COMMAND find ARGS ${PREFIX} -iname "*egg-info" -delete
    COMMAND rm ARGS -f /tmp/glite-wms-server-${PVER}-${AGE}.${OS}.tar.gz
    COMMAND ${PROJECT_SOURCE_DIR}/maketarball.sh ARGS ${PVER} ${AGE} ${OS} "${PREFIX}"
${PROJECT_SOURCE_DIR}
    COMMAND mkdir ARGS -p rpmbuild/SPECS rpmbuild/SRPMS rpmbuild/BUILD rpmbuild/RPMS rpmbuild/SOURCES
    COMMAND mv ARGS /tmp/glite-wms-server-${PVER}-${AGE}.${OS}.tar.gz rpmbuild/SOURCES/
    COMMAND sed ARGS -e 's/%{extversion}/${PVER}/g' -e 's/%{extage}/${AGE}/g'
-e 's/%{extdist}/${OS}/g' -e 's/%{extcdate}/${DATE}/g' -e 's/%{extclog}/Bug
fixing/g' < project/wms.spec > rpmbuild/SPECS/wms.spec
    COMMAND rpmbuild ARGS -ba --define "_topdir `pwd`/rpmbuild" --define
"extbuilddir ${PREFIX}" `pwd`/rpmbuild/SPECS/wms.spec
  )
  
  ADD_CUSTOM_COMMAND(TARGET configuration 
    COMMAND make ARGS -C configuration)
  ADD_CUSTOM_COMMAND(TARGET common 
    COMMAND make ARGS -C common)
  ADD_CUSTOM_COMMAND(TARGET purger 
    COMMAND make ARGS -C purger)
  ADD_CUSTOM_COMMAND(TARGET core 
    COMMAND make ARGS -C core)
  ADD_CUSTOM_COMMAND(TARGET jobsubmission 
    COMMAND make ARGS -C jobsubmission)
  ADD_CUSTOM_COMMAND(TARGET interface 
    COMMAND make ARGS -C interface)
  ADD_CUSTOM_COMMAND(TARGET ice 
    COMMAND make ARGS -C ice)
  
  
  ADD_CUSTOM_COMMAND(TARGET distclean
    COMMAND make ARGS clean
    COMMAND find ARGS ${FASTLIB_SOURCE_DIR} -iname CMakeCache.txt
-delete
    COMMAND find ARGS ${FASTLIB_SOURCE_DIR} -iname cmake_install.cmake
-delete
    COMMAND find ARGS ${FASTLIB_SOURCE_DIR} -iname Makefile -delete
    COMMAND find ARGS ${FASTLIB_SOURCE_DIR} -depth -type d -iname
CMakeFiles -exec rm -rf {} \;
    COMMAND rm ARGS -rf ${PREFIX}/stage install_manifest.txt
    COMMAND rm ARGS -rf ./interface/src/server/autogen
    COMMAND rm ARGS -rf ./core/src/helper/broker/autogen
    COMMAND rm ARGS ./configuration/config/glite-wms-check-daemons.sh
    ./configuration/config/glite-wms-parse-configuration.sh 
    ./ice/config/glite-wms-ice
    ./jobsubmission/config/glite-wms-clean-lm-recycle.sh 
    ./jobsubmission/config/glite-wms-jc 
    ./jobsubmission/config/glite-wms-lm
    ./pkgconfig/wms-server.pc
    VERBATIM )
ENDIF(UNIX)


set(BINDIR "bin")

set(SBINDIR "sbin")

set(LIBEXEC "libexec")

set(ENV{PKG_CONFIG_PATH}
"${PREFIX}/usr/lib/pkgconfig/:${PREFIX}/stage/usr/lib64/pkgconfig/:$ENV{PKG_CONFIG_PATH}")

if(DEFINED ADDITIONAL_PKG_CONFIG_PATH)
  set(ENV{PKG_CONFIG_PATH}
"${ADDITIONAL_PKG_CONFIG_PATH}:$ENV{PKG_CONFIG_PATH}")
endif(DEFINED ADDITIONAL_PKG_CONFIG_PATH)

set(BOOST_ROOT "/usr/lib64:/usr/lib")

find_package(PkgConfig REQUIRED)
find_package(Perl REQUIRED)
find_program(Make REQUIRED)
find_package(Boost REQUIRED COMPONENTS date_time filesystem thread regex program_options)

pkg_check_modules(CLASSAD REQUIRED emi-classad-ns)
pkg_check_modules(WMSUTILS_CLASSADS REQUIRED classad-utils)
pkg_check_modules(WMSUTILS_JOBMAN_EXCEPTION REQUIRED jobman-exception)
pkg_check_modules(GLOBUS_FTP_CLIENT REQUIRED globus-ftp-client)
pkg_check_modules(GLOBUS_IO REQUIRED globus-io)
pkg_check_modules(GLOBUS_GSS_ASSIST REQUIRED globus-gss-assist)
pkg_check_modules(GLOBUS_OPENSSL REQUIRED globus-openssl-module)
pkg_check_modules(GLOBUS_COMMON REQUIRED globus-common)
pkg_check_modules(JOBID REQUIRED emi-jobid)
pkg_check_modules(RENEWAL REQUIRED emi-proxyrenew)
pkg_check_modules(LOG4CPP REQUIRED log4cpp)
pkg_check_modules(CARES REQUIRED libcares)
pkg_check_modules(GSOAP REQUIRED gsoap)
pkg_check_modules(GSOAP_PLUSPLUS REQUIRED gsoap++)
pkg_check_modules(GSOAP_SSL REQUIRED gsoapssl)
pkg_check_modules(GSOAP_SSL_PLUSPLUS REQUIRED gsoapssl++)
pkg_check_modules(GSOAP_PLUGIN REQUIRED glite-security-gsoap-plugin++)
pkg_check_modules(JDL_API_CPP REQUIRED jdl-api-cpp)
pkg_check_modules(LB_CLIENT REQUIRED emi-lb-client)
pkg_check_modules(LB_CLIENTPP REQUIRED emi-lb-clientpp)
pkg_check_modules(VOMS REQUIRED voms-2.0)
pkg_check_modules(GLOBUS_GSSAPI REQUIRED globus-gssapi-gsi)
pkg_check_modules(ARGUSPEP REQUIRED libargus-pep)
pkg_check_modules(FCGI_CPP REQUIRED emi-fcgi-cpp)
pkg_check_modules(LCMAPS_RET_ACCT REQUIRED lcmaps-return-account-from-pem )
pkg_check_modules(LCMAPS_RET_POOLIDX REQUIRED lcmaps-return-poolindex )
pkg_check_modules(LCMAPS_VER_ACCT REQUIRED lcmaps-verify-account-from-pem )
pkg_check_modules(LCMAPS_GSS_ASSIST REQUIRED lcmaps-gss-assist-gridmap )
pkg_check_modules(LCMAPS REQUIRED lcmaps )
pkg_check_modules(XSLT REQUIRED libxslt )
pkg_check_modules(EXSLT REQUIRED libexslt )
pkg_check_modules(LCMAPS_GSS_ASSIST_WITHOUT_GSI REQUIRED
lcmaps-gss-assist-gridmap-without-gsi )
pkg_check_modules(LCMAPS_RET_POOLIDX_WITHOUT_GSI REQUIRED
lcmaps-return-poolindex-without-gsi )
pkg_check_modules(LCMAPS_WITHOUT_GSI REQUIRED lcmaps-without-gsi)
pkg_check_modules(LB_COMMON REQUIRED emi-lb-common)
pkg_check_modules(GLOBUS_GRAM_PROTO REQUIRED globus-gram-protocol)
pkg_check_modules(LIBXSLT REQUIRED libxslt)
pkg_check_modules(CONDOR REQUIRED condor-emi)
pkg_check_modules(GRIDSITE_OPENSSL REQUIRED gridsite-openssl)
pkg_check_modules(LIBXML REQUIRED libxml-2.0 )
pkg_check_modules(CREAM_CLIENT_UTIL REQUIRED cream-client-api-soap )
pkg_check_modules(CREAM_CLIENT_SOAP REQUIRED cream-client-api-util )

FIND_PROGRAM(PYTHON_EXECUTABLE
	NAMES python
	PATHS /usr/bin /usr/local/bin /opt/local/bin 
	)

if(NOT "${PYTHON_EXECUTABLE}" STREQUAL "PYTHON_EXECUTABLE-NOTFOUND"  )
  set(OKPYTHON "1")
endif(NOT "${PYTHON_EXECUTABLE}" STREQUAL "PYTHON_EXECUTABLE-NOTFOUND"  )

find_file(LDAP "ldap.h"
          PATHS "/usr/include")

find_file(LIBTAR "libtar.h"
          PATHS "/usr/include")

find_file(RPMBUILD "rpmbuild"
          PATHS "/usr/bin")

find_file(CHRPATH "chrpath"
          PATHS "/usr/bin")

find_file(HTTPD "httpd.h"
	  PATHS "/usr/include/httpd")

IF( NOT(HTTPD AND CHRPATH AND RPMBUILD AND LIBTAR AND LDAP AND CLASSAD_FOUND AND
WMSUTILS_CLASSADS_FOUND AND OKPYTHON AND OK_C_COMPILER AND OK_CXX_COMPILER AND 
WMSUTILS_JOBMAN_EXCEPTION_FOUND AND
GLOBUS_FTP_CLIENT_FOUND AND
GLOBUS_IO_FOUND AND
GLOBUS_GSS_ASSIST_FOUND AND
GLOBUS_OPENSSL_FOUND AND
GLOBUS_COMMON_FOUND AND
JOBID_FOUND AND
RENEWAL_FOUND AND
LOG4CPP_FOUND AND
CARES_FOUND AND
GSOAP_FOUND AND
GSOAP_PLUSPLUS_FOUND AND
GSOAP_SSL_FOUND AND
GSOAP_SSL_PLUSPLUS_FOUND AND
GSOAP_PLUGIN_FOUND AND
JDL_API_CPP_FOUND AND
LB_CLIENT_FOUND AND
LB_CLIENTPP_FOUND AND
VOMS_FOUND AND
GLOBUS_GSSAPI_FOUND AND
ARGUSPEP_FOUND AND
FCGI_CPP_FOUND AND
LCMAPS_RET_ACCT_FOUND AND
LCMAPS_RET_POOLIDX_FOUND AND
LCMAPS_VER_ACCT_FOUND AND
LCMAPS_GSS_ASSIST_FOUND AND
LCMAPS_FOUND AND
XSLT_FOUND AND
EXSLT_FOUND AND
LCMAPS_GSS_ASSIST_WITHOUT_GSI_FOUND AND
LCMAPS_RET_POOLIDX_WITHOUT_GSI_FOUND AND
LCMAPS_WITHOUT_GSI_FOUND AND
LB_COMMON_FOUND AND
GLOBUS_GRAM_PROTO_FOUND AND
LIBXSLT_FOUND AND
CONDOR_FOUND AND
GRIDSITE_OPENSSL_FOUND AND
LIBXML_FOUND AND
CREAM_CLIENT_UTIL_FOUND AND
CREAM_CLIENT_SOAP_FOUND AND Boost_FOUND) ) 

  message( WARNING " *** Some dependencies are missing. Will try to install
them via yum. Please insert your password" )
  set(RERUN "1")
  EXEC_PROGRAM(sudo ARGS yum -y install 
argus-pep-api-c
argus-pep-api-c-devel
boost.x86_64
boost-devel.x86_64
c-ares
c-ares-devel
chrpath
classads
classads-devel
condor-emi
docbook-style-xsl
emi-pkgconfig-compat
fcgi
fcgi-devel
glite-ce-cream-client-api-c
glite-ce-cream-client-devel
glite-jdl-api-cpp
glite-jdl-api-cpp-devel
glite-jobid-api-c
glite-jobid-api-c-devel
glite-jobid-api-cpp-devel
glite-lb-client
glite-lb-client-devel
glite-lb-common
glite-lb-common-devel
glite-px-proxyrenewal-devel
glite-px-proxyrenewal-libs
glite-wms-utils-classad
glite-wms-utils-classad-devel
glite-wms-utils-exception
glite-wms-utils-exception-devel
globus-common
globus-common-devel
globus-ftp-client
globus-ftp-client-devel
globus-gram-protocol
globus-gram-protocol-devel
globus-gssapi-gsi
globus-gssapi-gsi-devel
globus-gss-assist
globus-gss-assist-devel
globus-io
globus-io-devel
globus-openssl-module
globus-openssl-module-devel
gridsite
gridsite-devel
gsoap
gsoap-devel
httpd-devel
lcmaps
lcmaps-basic-interface
lcmaps-devel
lcmaps-globus-interface
lcmaps-openssl-interface
lcmaps-without-gsi
lcmaps-without-gsi-devel
libtar-devel
libxml2
libxml2-devel
libxslt
libxslt-devel
log4cpp
log4cpp-devel
openldap-devel
rpm-build
voms
voms-devel
python
swig
gcc
gcc-c++
libstdc++
make
  )
  
ENDIF( NOT(HTTPD AND CHRPATH AND RPMBUILD AND LIBTAR AND LDAP AND CLASSAD_FOUND AND
WMSUTILS_CLASSADS_FOUND AND OKPYTHON AND OK_C_COMPILER AND OK_CXX_COMPILER AND 
WMSUTILS_JOBMAN_EXCEPTION_FOUND AND
GLOBUS_FTP_CLIENT_FOUND AND
GLOBUS_IO_FOUND AND
GLOBUS_GSS_ASSIST_FOUND AND
GLOBUS_OPENSSL_FOUND AND
GLOBUS_COMMON_FOUND AND
JOBID_FOUND AND
RENEWAL_FOUND AND
LOG4CPP_FOUND AND
CARES_FOUND AND
GSOAP_FOUND AND
GSOAP_PLUSPLUS_FOUND AND
GSOAP_SSL_FOUND AND
GSOAP_SSL_PLUSPLUS_FOUND AND
GSOAP_PLUGIN_FOUND AND
JDL_API_CPP_FOUND AND
LB_CLIENT_FOUND AND
LB_CLIENTPP_FOUND AND
VOMS_FOUND AND
GLOBUS_GSSAPI_FOUND AND
ARGUSPEP_FOUND AND
FCGI_CPP_FOUND AND
LCMAPS_RET_ACCT_FOUND AND
LCMAPS_RET_POOLIDX_FOUND AND
LCMAPS_VER_ACCT_FOUND AND
LCMAPS_GSS_ASSIST_FOUND AND
LCMAPS_FOUND AND
XSLT_FOUND AND
EXSLT_FOUND AND
LCMAPS_GSS_ASSIST_WITHOUT_GSI_FOUND AND
LCMAPS_RET_POOLIDX_WITHOUT_GSI_FOUND AND
LCMAPS_WITHOUT_GSI_FOUND AND
LB_COMMON_FOUND AND
GLOBUS_GRAM_PROTO_FOUND AND
LIBXSLT_FOUND AND
CONDOR_FOUND AND
GRIDSITE_OPENSSL_FOUND AND
LIBXML_FOUND AND
CREAM_CLIENT_UTIL_FOUND AND
CREAM_CLIENT_SOAP_FOUND AND Boost_FOUND))

add_definitions( -Wall -pipe -DHAVE_STRINGSTREAM=1 -Wno-deprecated)

if(DEFINED BUILD_DBG)
  add_definitions( -g -O0 )
else(DEFINED BUILD_DBG)
  add_definitions( -O2 )
endif(DEFINED BUILD_DBG)

set(MAIN_PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}")

#if(NOT DEFINED SYSCONFIGDIR)
set(SYSCONFIGDIR "${PREFIX}/etc/")
#endif(NOT DEFINED SYSCONFIGDIR)

install(FILES
        LICENSE
        DESTINATION ${PREFIX}/usr/share/doc/glite-wms-server-${PVER}/
)

add_subdirectory(configuration)
add_subdirectory(common)
add_subdirectory(purger)
add_subdirectory(core)
add_subdirectory(jobsubmission)
add_subdirectory(interface)
add_subdirectory(ice)
add_subdirectory(pkgconfig)
add_subdirectory(nagios)
if(RERUN)
message(WARNING " Please re-run cmake once again...")
endif(RERUN)
