
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

add_definitions( ${LIBTAR_CFLAGS} -DALLOW_COMMAND_LINE_OVERRIDE )

link_directories(${PREFIX}/usr/${LIBDIR})

add_executable(glite_wms_wmproxy_dirmanager
  glite-wms-wmproxy-dirmanager.cpp
)

target_link_libraries(glite_wms_wmproxy_dirmanager
  ${LIBTAR_LDFLAGS}
  -ltar
  -lz
)

#set_target_properties(glite_wms_wmproxy_eventlogger PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS 
  glite_wms_wmproxy_dirmanager
  DESTINATION 
  ${PREFIX}/usr/${LIBEXEC}
  )
