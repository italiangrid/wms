project(wms-ice C CXX)
cmake_minimum_required(VERSION 2.6)

if(NOT DEFINED PVER)
    message(WARNING " Undefined version number, selected 0.0.0. Please use
-DPVER=<version> to customize")
    set(PVER "0.0.0")
endif(NOT DEFINED PVER)

if(NOT DEFINED PREFIX)
    message(WARNING " Undefined prefix, selected /usr/local")
    set(PREFIX "/usr/local")
endif(NOT DEFINED PREFIX)

if(NOT DEFINED PKG_INSTALL_PREFIX)
    set(PKG_INSTALL_PREFIX ${PREFIX})
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

IF (UNIX)
  # since it's unix-specific we will use bash
  ADD_CUSTOM_TARGET (distclean @echo cleaning ${FASTLIB_SOURCE_DIR}
for source distribution)
        
  ADD_CUSTOM_COMMAND(TARGET distclean
    COMMAND make ARGS clean
    COMMAND find ARGS ${FASTLIB_SOURCE_DIR} -iname CMakeCache.txt
-delete
    COMMAND find ARGS ${FASTLIB_SOURCE_DIR} -iname cmake_install.cmake
-delete
    COMMAND find ARGS ${FASTLIB_SOURCE_DIR} -iname Makefile -delete
    COMMAND find ARGS ${FASTLIB_SOURCE_DIR} -depth -type d -iname
CMakeFiles -exec rm -rf {} \;
    COMMAND rm ARGS -rf bin lib include
    VERBATIM )
ENDIF(UNIX)

set(BINDIR "bin")

set(SBINDIR "sbin")

set(LIBEXEC "libexec")

execute_process(COMMAND "date" RESULT_VARIABLE ICE_VERSIONID)

set(ENV{PKG_CONFIG_PATH}
"${PREFIX}/lib/pkgconfig/:${PREFIX}/lib64/pkgconfig/:$ENV{PKG_CONFIG_PATH}")

if(DEFINED ADDITIONAL_PKG_CONFIG_PATH)
  set(ENV{PKG_CONFIG_PATH}
"${ADDITIONAL_PKG_CONFIG_PATH}:$ENV{PKG_CONFIG_PATH}")
endif(DEFINED ADDITIONAL_PKG_CONFIG_PATH)

set(BOOST_ROOT "/usr/lib64:/usr/lib" )

find_package(PkgConfig REQUIRED)
find_package(Perl REQUIRED)
find_program(Make REQUIRED)
pkg_check_modules(CLASSAD REQUIRED emi-classad-ns)
pkg_check_modules(JDL_API_CPP REQUIRED jdl-api-cpp)
pkg_check_modules(WMS_COMMON_CONF REQUIRED wms-common-conf)
pkg_check_modules(WMS_COMMON_UTIL REQUIRED wms-common-util)
pkg_check_modules(PURGER REQUIRED wms-purger)
pkg_check_modules(LB_COMMON REQUIRED emi-lb-common)
pkg_check_modules(LB_CLIENTPP REQUIRED emi-lb-clientpp)
pkg_check_modules(JDL_API_CPP REQUIRED jdl-api-cpp)
pkg_check_modules(JOBID REQUIRED emi-jobid)
pkg_check_modules(FCGI_CPP REQUIRED emi-fcgi-cpp)
pkg_check_modules(GRIDSITE_OPENSSL REQUIRED gridsite-openssl)
pkg_check_modules(VOMS REQUIRED voms-2.0)
pkg_check_modules( RENEWAL REQUIRED emi-proxyrenew )
pkg_check_modules( GSOAP_PP REQUIRED gsoap++ )
pkg_check_modules( GSOAPSSL_PP REQUIRED gsoapssl++ )
pkg_check_modules( LIBXML REQUIRED libxml-2.0 )
pkg_check_modules( CREAM_CLIENT_UTIL REQUIRED cream-client-api-soap )
pkg_check_modules( CREAM_CLIENT_SOAP REQUIRED cream-client-api-util )
pkg_check_modules( GSOAP_PLUGINPP glite-security-gsoap-plugin++ )
find_package( Boost COMPONENTS program_options REQUIRED)

if(EXISTS "/etc/redhat-release")
  set(ONRH "1")
endif(EXISTS "/etc/redhat-release")

if(DEFINED ONRH)
EXEC_PROGRAM(cat
  ARGS  /etc/redhat-release | /usr/bin/awk '{print $(NF-1)}' | /usr/bin/awk -F"." '{print $1}'
  OUTPUT_VARIABLE RH_VER)
endif(DEFINED ONRH)

add_definitions( -DICE_VERSIONID=${ICE_VERSIONID} -DHAVE_GLITE_JOBID -DGLITE_WMS_ICE_ENABLE_STATS -UHAVE_CONFIG_H  -DHAVE_STRINGSTREAM -pipe -Wall ${CLASSAD_CFLAGS} )

if(DEFINED BUILD_DBG)
  add_definitions( -g -O0 )
else(DEFINED BUILD_DBG)
  
  add_definitions( -O2 )
  
endif(DEFINED BUILD_DBG)

install(FILES
        LICENSE
        DESTINATION ${PREFIX}/share/doc/glite-wms-ice-${PVER}/
)

if(NOT DEFINED SYSCONFIGDIR)
    set(SYSCONFIGDIR "${PREFIX}/../etc/")
endif(NOT DEFINED SYSCONFIGDIR)

add_subdirectory(src)
add_subdirectory(config)
