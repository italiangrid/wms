project(wms-purger CXX)
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
pkg_check_modules(WMS_COMMON_UTIL REQUIRED wms-common-util)
#pkg_check_modules(WMS_COMMON_QUOTA REQUIRED wms-common-quota)
#pkg_check_modules(WMS_COMMON_PROCESS REQUIRED wms-common-process)
#pkg_check_modules(WMS_COMMON REQUIRED wms-common)
pkg_check_modules(WMS_COMMON_LOGGER REQUIRED wms-common-logger)
#pkg_check_modules(WMS_COMMON_II REQUIRED wms-common-ii)
pkg_check_modules(WMS_COMMON_CONF REQUIRED wms-common-conf)
pkg_check_modules(WMSUTILS_CLASSADS REQUIRED classad-utils)
pkg_check_modules(GLOBUS_GSS_ASSIST REQUIRED globus-gss-assist)
pkg_check_modules(GLOBUS_OPENSSL REQUIRED globus-openssl-module)
pkg_check_modules(GLOBUS_COMMON REQUIRED globus-common)
pkg_check_modules(LB_CLIENT REQUIRED emi-lb-client)
pkg_check_modules(LB_CLIENTPP REQUIRED emi-lb-clientpp)
pkg_check_modules(LB_COMMON REQUIRED emi-lb-common)
pkg_check_modules(JOBID REQUIRED emi-jobid)
pkg_check_modules(CLASSAD REQUIRED emi-classad-ns)
pkg_check_modules(CARES REQUIRED libcares)
pkg_check_modules(WMSUTILS_JOBMAN_EXCEPTION REQUIRED jobman-exception)
pkg_check_modules(RENEWAL REQUIRED emi-proxyrenew)
pkg_check_modules(LOG4CPP REQUIRED log4cpp)
find_package( Boost COMPONENTS date_time filesystem regex thread
program_options REQUIRED)

if(EXISTS "/etc/debian_version")
  set(ONDEB "1")
endif(EXISTS "/etc/debian_version")



if(EXISTS "/etc/redhat-release")
  set(ONRH "1")
endif(EXISTS "/etc/redhat-release")



if(DEFINED ONDEB)
EXEC_PROGRAM(cat
  ARGS /etc/debian_version | /usr/bin/awk -F"." '{print $1}'
  OUTPUT_VARIABLE DEB_VER)
endif(DEFINED ONDEB)



if(DEFINED ONRH)
EXEC_PROGRAM(cat
  ARGS  /etc/redhat-release | /usr/bin/awk '{print $(NF-1)}' | /usr/bin/awk -F"." '{print $1}'
  OUTPUT_VARIABLE RH_VER)
endif(DEFINED ONRH)

# this is an hack: emi-boost-thread provides ldflag -lboost_thread; SL5 has libboost_thread.so, but SL6 has libboost_thread-mt (v. 1.41 instead of 1.33)
set(BOOST_THREAD_HACK_LDFLAGS "-lboost_thread-mt")
if("${RH_VER}" STREQUAL "5") 
	set(BOOST_THREAD_HACK_LDFLAGS "-lboost_thread")
endif("${RH_VER}" STREQUAL "5") 

add_definitions( -DHAVE_STRINGSTREAM=1 )

if(DEFINED BUILD_DBG)

  add_definitions( -pipe -g -O0 -Wall ${CLASSAD_CFLAGS})


else(DEFINED BUILD_DBG)

  add_definitions( -pipe -O2 -Wall ${CLASSAD_CFLAGS})

endif(DEFINED BUILD_DBG)

install(FILES
        LICENSE
        DESTINATION ${PREFIX}/share/doc/glite-wms-purger-${PVER}/
)

add_subdirectory(src)
add_subdirectory(config)
