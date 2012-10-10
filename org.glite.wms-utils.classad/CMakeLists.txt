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
set(ENV{PKG_CONFIG_PATH}
"${PREFIX}/${LIBDIR}/pkgconfig/:$ENV{PKG_CONFIG_PATH}")

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/emi-classad-ns.pc
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR}/pkgconfig)


set(CMAKE_INSTALL_PREFIX ${PREFIX})




find_package(PkgConfig REQUIRED)
pkg_check_modules(CLASSAD emi-classad-ns)

add_definitions( -pipe -DWANT_NAMESPACES -DWANT_CLASSAD_NAMESPACE )

add_subdirectory(src)
add_subdirectory(interface)
