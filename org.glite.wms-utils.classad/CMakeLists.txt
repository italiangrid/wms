project(wmsutils-classad CXX)
cmake_minimum_required(VERSION 2.6)

if(NOT DEFINED PVER)
    message(WARNING " Undefined version number, selected 9.9.9")
    set(PVER "9.9.9")
endif(NOT DEFINED PVER)

if( IS_DIRECTORY "/usr/lib64" )
    set(LIBDIR "lib64")
else( IS_DIRECTORY "/usr/lib" )
    set(LIBDIR "lib")
endif( IS_DIRECTORY "/usr/lib64" )

if(NOT DEFINED OFFICIAL_INSTALL_PREFIX)
    set(OFFICIAL_INSTALL_PREFIX ${PREFIX})
endif(NOT DEFINED OFFICIAL_INSTALL_PREFIX)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")

find_package(PkgConfig REQUIRED)
pkg_check_modules(CLASSAD emi-classad-ns)

set(CMAKE_INSTALL_PREFIX ${PREFIX})
add_definitions( -pipe -DWANT_NAMESPACES -DWANT_CLASSAD_NAMESPACE )

add_subdirectory(src)
add_subdirectory(interface)
