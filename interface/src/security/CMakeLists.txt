
include_directories(
  ${VOMS_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${PURGER_INCLUDE_DIRS}
  ${WMS_COMMON_LOGGER_INCLUDE_DIRS}
  ${WMS_COMMON_UTIL_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src
  ${MAIN_PROJECT_SOURCE_DIR}
  ${MAIN_PROJECT_SOURCE_DIR}/common/interface
  ${ARGUSPEP_INCLUDE_DIRS}
  ${LCMAPS_INCLUDE_DIRS}
  )

link_directories(${PREFIX}/usr/${LIBDIR})


add_library(glite_wms_wmproxy_security
  SHARED 
  authorizer.cpp
  delegation.cpp
  argus_authz.cpp
  gaclmanager.cpp
  vomsauthn.cpp
 
  )

set_target_properties(glite_wms_wmproxy_security PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_wmproxy_security
  LIBRARY DESTINATION ${PREFIX}/usr/${LIBDIR})
