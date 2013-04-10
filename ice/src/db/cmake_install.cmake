# Install script for directory: /home/dorigoa/wms/ice/src/db

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF(EXISTS "$ENV{DESTDIR}/home/dorigoa/wms/stage/usr/lib64/libglite_wms_ice_db.so.0.0.0")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/dorigoa/wms/stage/usr/lib64/libglite_wms_ice_db.so.0.0.0"
         RPATH "")
  ENDIF(EXISTS "$ENV{DESTDIR}/home/dorigoa/wms/stage/usr/lib64/libglite_wms_ice_db.so.0.0.0")
  FILE(INSTALL DESTINATION "/home/dorigoa/wms/stage/usr/lib64" TYPE SHARED_LIBRARY FILES
    "/home/dorigoa/wms/ice/src/db/libglite_wms_ice_db.so.0.0.0"
    "/home/dorigoa/wms/ice/src/db/libglite_wms_ice_db.so.0"
    "/home/dorigoa/wms/ice/src/db/libglite_wms_ice_db.so"
    )
  IF(EXISTS "$ENV{DESTDIR}/home/dorigoa/wms/stage/usr/lib64/libglite_wms_ice_db.so.0.0.0")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/dorigoa/wms/stage/usr/lib64/libglite_wms_ice_db.so.0.0.0")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF(EXISTS "$ENV{DESTDIR}/home/dorigoa/wms/stage/usr/lib64/libglite_wms_ice_db.so.0.0.0")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/dorigoa/wms/ice/src/db/sqlite/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

