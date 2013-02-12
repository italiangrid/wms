project(wms-common CXX)
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



set(ENV{PKG_CONFIG_PATH}
"${PREFIX}/lib/pkgconfig/:${PREFIX}/lib64/pkgconfig/:$ENV{PKG_CONFIG_PATH}")

if(DEFINED ADDITIONAL_PKG_CONFIG_PATH)
  set(ENV{PKG_CONFIG_PATH}
"${ADDITIONAL_PKG_CONFIG_PATH}:$ENV{PKG_CONFIG_PATH}")
endif(DEFINED ADDITIONAL_PKG_CONFIG_PATH)

set(BOOST_ROOT "/usr/lib64:/usr/lib")

find_package(PkgConfig REQUIRED)
find_package(Perl REQUIRED)
find_program(Make REQUIRED)
pkg_check_modules(CLASSAD REQUIRED emi-classad-ns)
pkg_check_modules(JOBID REQUIRED emi-jobid)
pkg_check_modules(WMSUTILS_CLASSADS REQUIRED classad-utils)
pkg_check_modules(WMSUTILS_JOBMAN_EXCEPTION REQUIRED jobman-exception)
pkg_check_modules(GLOBUS_COMMON REQUIRED globus-common)
pkg_check_modules(GLOBUS_FTP_CLIENT REQUIRED globus-ftp-client)
pkg_check_modules(GLOBUS_GSS_ASSIST REQUIRED globus-gss-assist)
pkg_check_modules(GLOBUS_IO REQUIRED globus-io)
pkg_check_modules(LOG4CPP REQUIRED log4cpp)
find_package( Boost COMPONENTS date_time filesystem regex thread
program_options REQUIRED)

add_definitions( -Wall -pipe -DHAVE_STRINGSTREAM=1 )

if(DEFINED BUILD_DBG)
  
  add_definitions( -g -O0 ${CLASSAD_CFLAGS})
  
  
else(DEFINED BUILD_DBG)
  
  add_definitions( -O2 ${CLASSAD_CFLAGS})
  
endif(DEFINED BUILD_DBG)

install(FILES
        LICENSE
        DESTINATION ${PREFIX}/share/doc/glite-wms-common-${PVER}/
)


add_subdirectory(src)
add_subdirectory(interface)
