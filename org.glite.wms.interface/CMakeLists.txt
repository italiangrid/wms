project(wms-interface C CXX)
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
pkg_check_modules(VOMS REQUIRED voms-2.0)
pkg_check_modules(CLASSAD REQUIRED emi-classad-ns)
pkg_check_modules(WMSUTILS_JOBMAN_EXCEPTION REQUIRED jobman-exception)
pkg_check_modules(JDL_API_CPP REQUIRED jdl-api-cpp)
pkg_check_modules(WMS_COMMON_CONF REQUIRED wms-common-conf)
pkg_check_modules(WMS_COMMON_LOGGER REQUIRED wms-common-logger)
pkg_check_modules(WMS_COMMON_UTIL REQUIRED wms-common-util)
pkg_check_modules(WMS_COMMON_QUOTA REQUIRED wms-common-quota)
pkg_check_modules(PURGER REQUIRED wms-purger)
pkg_check_modules(GLOBUS_GSSAPI REQUIRED globus-gssapi-gsi)
pkg_check_modules(LCMAPS REQUIRED lcmaps)
pkg_check_modules(ARGUSPEP REQUIRED libargus-pep)
pkg_check_modules(ZLIB REQUIRED zlib)
find_package( Boost COMPONENTS date_time filesystem regex thread program_options)

if(EXISTS "/etc/debian_version")
  set(ONDEB "1")
endif(EXISTS "/etc/debian_version")



if(EXISTS "/etc/redhat-release")
  set(ONRH "1")
endif(EXISTS "/etc/redhat-release")


if(DEFINED ONDEB)
  add_definitions( -DGSOAP_279_TRICK )
endif(DEFINED ONDEB)

if(DEFINED BUILD_DBG)
  
  add_definitions( -pipe -g -O0 -Wall ${CLASSAD_CFLAGS} -DLDAP_DEPRECATED)
  
  
else(DEFINED BUILD_DBG)
  
  add_definitions( -pipe -O2 -Wall ${CLASSAD_CFLAGS} -DLDAP_DEPRECATED)
  
endif(DEFINED BUILD_DBG)

install(FILES
        LICENSE
        DESTINATION ${PREFIX}/share/doc/glite-wms-interface-${PVER}/
)

if(NOT DEFINED SYSCONFIGDIR)
    set(SYSCONFIGDIR "etc/")
endif(NOT DEFINED SYSCONFIGDIR)


add_subdirectory(src)
#add_subdirectory(doc)
add_subdirectory(config)
