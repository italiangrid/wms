
include_directories(
  ${CLASSAD_INCLUDE_DIRS}
  ${WMS_COMMON_LOGGER_INCLUDE_DIRS}
  ${WMS_COMMON_CONF_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src
  ${WMSUTILS_JOBMAN_EXCEPTION_INCLUDE_DIRS}
  ${JDL_API_CPP_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${GLOBUS_GSSAPI_INCLUDE_DIRS}
  ${ARGUSPEP_INCLUDE_DIRS}
  ${LCMAPS_INCLUDE_DIRS}
  )

link_directories(${PREFIX}/${LIBDIR})

add_library(glite_wms_wmproxy_commands
  SHARED 
  listfiles.cpp
  listjobmatch.cpp
  )

add_library(glite_wms_wmproxy_pipe
  SHARED 
  MatchingPipe_nb.cpp
  MatchingPipe.cpp
  )


set_target_properties(glite_wms_wmproxy_commands PROPERTIES VERSION 0.0.0 SOVERSION 0)
set_target_properties(glite_wms_wmproxy_pipe PROPERTIES VERSION 0.0.0 SOVERSION 0)


install(TARGETS 
  glite_wms_wmproxy_commands
  glite_wms_wmproxy_pipe
  LIBRARY 
  DESTINATION 
  ${PREFIX}/${LIBDIR})
