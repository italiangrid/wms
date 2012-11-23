project(wms-core CXX)
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

set(BINDIR "bin")

set(SBINDIR "sbin")

set(LIBEXEC "libexec")



set(ENV{PKG_CONFIG_PATH}
"${PREFIX}/lib/pkgconfig/:${PREFIX}/lib64/pkgconfig/:$ENV{PKG_CONFIG_PATH}")

if(DEFINED ADDITIONAL_PKG_CONFIG_PATH)
  set(ENV{PKG_CONFIG_PATH}
"${ADDITIONAL_PKG_CONFIG_PATH}:$ENV{PKG_CONFIG_PATH}")
endif(DEFINED ADDITIONAL_PKG_CONFIG_PATH)



find_package(PkgConfig REQUIRED)
find_package(Perl REQUIRED)
find_program(Make REQUIRED)
pkg_check_modules(CARES libcares)
pkg_check_modules(GSOAP REQUIRED gsoap)
pkg_check_modules(GSOAP_PLUSPLUS REQUIRED gsoap++)
pkg_check_modules(GSOAP_SSL REQUIRED gsoapssl)
pkg_check_modules(GSOAP_SSL_PLUSPLUS REQUIRED gsoapssl++)
pkg_check_modules(GSOAP_PLUGIN REQUIRED emi-gsoap-plugin)
pkg_check_modules(BOOST_REGEX REQUIRED emi-boost-regex)
pkg_check_modules(BOOST_FILESYSTEM REQUIRED emi-boost-filesystem)
pkg_check_modules(BOOST_THREAD REQUIRED REQUIRED emi-boost-thread)
pkg_check_modules(BOOST_OPTIONS REQUIRED emi-boost-options)
pkg_check_modules(CLASSAD REQUIRED emi-classad-ns)
pkg_check_modules(WMSUTILS_CLASSADS REQUIRED classad-utils)
pkg_check_modules(WMSUTILS_JOBMAN_EXCEPTION REQUIRED jobman-exception)
pkg_check_modules(JDL_API_CPP REQUIRED jdl-api-cpp)
pkg_check_modules(WMS_COMMON_CONF REQUIRED wms-common-conf)
pkg_check_modules(WMS_COMMON_LOGGER REQUIRED wms-common-logger)
pkg_check_modules(WMS_COMMON_PROCESS REQUIRED wms-common-process)
pkg_check_modules(WMS_COMMON_UTIL REQUIRED wms-common-util)
pkg_check_modules(WMS_COMMON_II REQUIRED wms-common-ii)
pkg_check_modules(WMS_COMMON_QUOTA REQUIRED wms-common-quota)
pkg_check_modules(LB_COMMON REQUIRED emi-lb-common)
pkg_check_modules(LB_CLIENT REQUIRED emi-lb-client)
pkg_check_modules(LB_CLIENTPP REQUIRED emi-lb-clientpp)
pkg_check_modules(PROXYRENEW REQUIRED emi-proxyrenew)
pkg_check_modules(PURGER REQUIRED wms-purger)
pkg_check_modules(JOBID emi-jobid)
pkg_check_modules(GLOBUS_GSSAPI globus-gssapi-gsi)

if(DEFINED BUILD_DBG)
  
  add_definitions( -pipe -g -O0 -Wall ${CLASSAD_CFLAGS} -DLDAP_DEPRECATED)
  
  
else(DEFINED BUILD_DBG)
  
  add_definitions( -pipe -O2 -Wall ${CLASSAD_CFLAGS} -DLDAP_DEPRECATED)
  
endif(DEFINED BUILD_DBG)

install(FILES
        LICENSE
        DESTINATION ${PREFIX}/share/doc/glite-wms-common-${PVER}/
)


#add_subdirectory(src)
add_subdirectory(src/ism)
add_subdirectory(src/helper/jobadapter)
add_subdirectory(src/helper/broker)
add_subdirectory(interface)
