project(wmsutils-classad CXX)
cmake_minimum_required(VERSION 2.6)

if(NOT DEFINED PVER)
    message(WARNING " Undefined version number, selected 9.9.9")
    set(PVER "9.9.9")
endif(NOT DEFINED PVER)

if(NOT DEFINED PREFIX)
    message(WARNING " Undefined prefix, selected /usr/local")
    set(PREFIX "/usr/local")
endif(NOT DEFINED PREFIX)

if(NOT DEFINED OFFICIAL_INSTALL_PREFIX)
    set(OFFICIAL_INSTALL_PREFIX ${PREFIX})
endif(NOT DEFINED OFFICIAL_INSTALL_PREFIX)


if( IS_DIRECTORY "/usr/lib64" )
    set(LIBDIR "lib64")
else( IS_DIRECTORY "/usr/lib" )
    set(LIBDIR "lib")
endif( IS_DIRECTORY "/usr/lib64" )

set(BINDIR "/bin")

set(LIBEXEC "libexec")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")

find_package(PkgConfig REQUIRED)

set(CMAKE_INSTALL_PREFIX ${PREFIX})
add_definitions( -pipe )

add_subdirectory(src)
add_subdirectory(interface)
