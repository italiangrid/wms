project(wms-ism CXX)
cmake_minimum_required(VERSION 2.6)

if(NOT DEFINED PVER)
    message(WARNING " Undefined version number, selected 9.9.9. Please use
-DPVER=<version> to customize")
    set(PVER "9.9.9")
endif(NOT DEFINED PVER)

if(NOT DEFINED PREFIX)
    message(WARNING " Undefined prefix, selected /usr/local")
    set(PREFIX "/usr/local")
endif(NOT DEFINED PREFIX)

if(NOT DEFINED OFFICIAL_INSTALL_PREFIX)
    set(OFFICIAL_INSTALL_PREFIX ${PREFIX})
endif(NOT DEFINED OFFICIAL_INSTALL_PREFIX)

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

set(LIBEXEC "libexec")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")

set(ENV{PKG_CONFIG_PATH}
"${PREFIX}/lib/pkgconfig:${PREFIX}/lib64/pkgconfig:$ENV{PKG_CONFIG_PATH}")

if(DEFINED ADDITIONAL_PKG_CONFIG_PATH)
  set(ENV{PKG_CONFIG_PATH}
"${ADDITIONAL_PKG_CONFIG_PATH}:$ENV{PKG_CONFIG_PATH}")
endif(DEFINED ADDITIONAL_PKG_CONFIG_PATH)


find_package(PkgConfig REQUIRED)
find_package(Perl REQUIRED)
find_program(Make REQUIRED)
pkg_check_modules(CLASSAD emi-classad-ns)
pkg_check_modules(BOOST emi-boost-thread)
pkg_check_modules(JOBID emi-jobid)
pkg_check_modules(WMSUTILS_CLASSADS classad-utils)
pkg_check_modules(WMSUTILS_JOBMAN_EXCEPTION jobman-exception)
pkg_check_modules(GLOBUS_COMMON globus-common)
pkg_check_modules(GLOBUS_FTP_CLIENT globus-ftp-client)
pkg_check_modules(GLOBUS_GSS_ASSIST globus-gss-assist)
pkg_check_modules(GLOBUS_IO globus-io)
pkg_check_modules(LOG4CPP log4cpp)
pkg_check_modules(WMS_COMMON_UTIL wms-common-util)
pkg_check_modules(WMS_COMMON_QUOTA wms-common-quota)
pkg_check_modules(WMS_COMMON_PROCESS wms-common-process)
pkg_check_modules(WMS_COMMON wms-common)
pkg_check_modules(WMS_COMMON_LOGGER wms-common-logger)
pkg_check_modules(WMS_COMMON_CONF wms-common-conf)

set(CMAKE_INSTALL_PREFIX ${PREFIX})
add_definitions( -pipe -DWANT_NAMESPACES -DWANT_CLASSAD_NAMESPACE
-DLDAP_DEPRECATED)

add_subdirectory(src)
add_subdirectory(interface)
