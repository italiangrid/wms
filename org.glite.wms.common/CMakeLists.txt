project(wms-common CXX)
cmake_minimum_required(VERSION 2.6)

if(NOT DEFINED PVER)
    message(WARNING " Undefined version number, selected 9.9.9")
    set(PVER "9.9.9")
endif(NOT DEFINED PVER)

if(NOT DEFINED PREFIX)
    message(WARNING " Undefined prefix, selected /usr/local")
    set(PREFIX "/usr/local")
endif(NOT DEFINED PREFIX)

if( IS_DIRECTORY "/usr/lib64" )
    set(LIBDIR "lib64")
else( IS_DIRECTORY "/usr/lib" )
    set(LIBDIR "lib")
endif( IS_DIRECTORY "/usr/lib64" )

set(BINDIR "/bin")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")

find_package(PkgConfig REQUIRED)
find_package(Perl REQUIRED)
find_program(Make REQUIRED)
pkg_check_modules(CLASSAD emi-classad-ns)
pkg_check_modules(BOOST emi-boost-thread)
pkg_check_modules(JOBID emi-jobid)
pkg_check_modules(WMSUTILS_CLASSADS glite-wmsutils-classads)
pkg_check_modules(WMSUTILS_JOBMAN_EXCEPTION glite-wmsutils-jobman-exception)
pkg_check_modules(GLOBUS_COMMON globus-common)
pkg_check_modules(GLOBUS_FTP_CLIENT globus-ftp-client)
pkg_check_modules(GLOBUS_GSS_ASSIST globus-gss-assist)
pkg_check_modules(GLOBUS_IO globus-io)
pkg_check_modules(LOG4CPP log4cpp)



set(CMAKE_INSTALL_PREFIX ${PREFIX})
add_definitions( -pipe -DWANT_NAMESPACES -DWANT_CLASSAD_NAMESPACE )

add_subdirectory(src)
add_subdirectory(interface)
