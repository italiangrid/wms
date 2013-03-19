
include_directories(
  ${CLASSAD_INCLUDE_DIRS}
  #                    ${PROJECT_SOURCE_DIR}/interface
  #		    ${PREFIX}/include
  #		    ${WMSUTILS_CLASSADS_ALT_INCLUDE_DIRS}
  #		    ${WMSUTILS_CLASSADS_INCLUDE_DIRS}
  #		    ${WMS_COMMON_UTIL_INCLUDE_DIRS}
  ${WMS_COMMON_LOGGER_INCLUDE_DIRS}
  ${WMS_COMMON_CONF_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src
  ${MAIN_PROJECT_SOURCE_DIR}/common/interface
  ${MAIN_PROJECT_SOURCE_DIR}/
  ${WMSUTILS_JOBMAN_EXCEPTION_INCLUDE_DIRS}
  ${JDL_API_CPP_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${GLOBUS_GSSAPI_INCLUDE_DIRS}
  ${ARGUSPEP_INCLUDE_DIRS}
  ${LCMAPS_INCLUDE_DIRS}
  )

link_directories(${PREFIX}/usr/${LIBDIR})


add_library(glite_wms_wmproxy_eventlogger
  SHARED 
  expdagad.cpp
  eventlogger.cpp
  )

set_target_properties(glite_wms_wmproxy_eventlogger PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_wmproxy_eventlogger
  LIBRARY DESTINATION ${PREFIX}/usr/${LIBDIR})
