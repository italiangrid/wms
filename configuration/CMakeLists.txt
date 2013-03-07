project(wms-configuration CXX)
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

install(FILES
  LICENSE
  DESTINATION ${PREFIX}/share/doc/glite-wms-configuration-${PVER}
)

add_subdirectory(config)
