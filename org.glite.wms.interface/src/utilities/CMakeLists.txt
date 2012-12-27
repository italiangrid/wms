
include_directories(
  ${CLASSAD_INCLUDE_DIRS}
  #                    ${PROJECT_SOURCE_DIR}/interface
  #		    ${PREFIX}/include
  #		    ${WMSUTILS_CLASSADS_ALT_INCLUDE_DIRS}
  #		    ${WMSUTILS_CLASSADS_INCLUDE_DIRS}
  #		    ${WMS_COMMON_UTIL_INCLUDE_DIRS}
  ${PURGER_INCLUDE_DIRS}
  ${WMS_COMMON_LOGGER_INCLUDE_DIRS}
  ${WMS_COMMON_UTIL_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src
  ${ARGUSPEP_INCLUDE_DIRS}
  ${LCMAPS_INCLUDE_DIRS}
  )

link_directories(${PREFIX}/${LIBDIR})


add_library(glite_wms_wmproxy_utilities
  SHARED 
  wmpexceptions.cpp 
  utils.cpp
  )

set_target_properties(glite_wms_wmproxy_utilities PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_wmproxy_utilities
  LIBRARY DESTINATION ${PREFIX}/${LIBDIR})
