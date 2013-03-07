
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
  ${LIBXML_INCLUDE_DIRS}
  )

add_custom_command(
  OUTPUT 
  ${PROJECT_SOURCE_DIR}/src/server/soapC.cpp 
  ${PROJECT_SOURCE_DIR}/src/server/soapServer.cpp
  #${PROJECT_SOURCE_DIR}/src/server/soapClient.cpp
  ${PROJECT_SOURCE_DIR}/src/server/wm.h 
  COMMAND
  wsdl2h -o ${PROJECT_SOURCE_DIR}/src/server/wm.h 
         -t ${PROJECT_SOURCE_DIR}/src/server/wmptypemap.dat 
         ${PROJECT_SOURCE_DIR}/src/server/WMProxy.wsdl
  COMMAND 
  soapcpp2 -I/usr/include:/usr/import:/usr/share/gsoap/import:/usr/include/gsoap ${PROJECT_SOURCE_DIR}/src/server/wm.h
  )

add_custom_target(GENSTUBS ALL DEPENDS 
  ${PROJECT_SOURCE_DIR}/src/server/soapC.cpp 
  ${PROJECT_SOURCE_DIR}/src/server/soapServer.cpp
  #${PROJECT_SOURCE_DIR}/src/server/soapClient.cpp
  ${PROJECT_SOURCE_DIR}/src/server/wm.h
  )

link_directories(${PREFIX}/${LIBDIR}
  ${PROJECT_SOURCE_DIR}/src/utilities
  ${PROJECT_SOURCE_DIR}/src/security
  ${PROJECT_SOURCE_DIR}/src/eventlogger
  ${PROJECT_SOURCE_DIR}/src/dirmanager
  ${PROJECT_SOURCE_DIR}/src/commands
  ${WMS_COMMON_LOGGER_LIBRARY_DIRS}
  ${PURGER_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS}
  )

add_executable(glite_wms_wmproxy_server
  wm.h
  soapC.cpp
  soapServer.cpp
  #soapClient.cpp
  wmproxy.cpp
  wmproxyserve.cpp
  common.cpp
  operations.cpp
  coreoperations.cpp
  gsoapfaultmanipulator.cpp
  gsoapoperations.cpp
  configuration.cpp
  wmp2wm.cpp
  structconverter.cpp
  stdsoap2-${GSOAP_VERSION}.cpp
  signalhandler.cpp
)

target_link_libraries(
  glite_wms_wmproxy_server
  ${WMS_COMMON_LOGGER_LIBRARIES}
  ${WMS_COMMON_CONF_LIBRARIES}
  ${WMS_COMMON_UTIL_LIBRARIES}
  ${WMS_COMMON_QUOTA_LIBRARIES}
  ${Boost_LIBRARIES}
  ${LB_COMMOND_LIBRARIES}
  ${JDL_API_CPP_LIBRARIES}
  ${JOBID_LIBRARIES}
  ${LB_CLIENTPP_LIBRARIES}
  ${FCGI_CPP_LIBRARIES}
  ${GRIDSITE_OPENSSL_LIBRARIES}
  ${VOMS_LIBRARIES}
  ${LCMAPS_WITHOUT_GSI_LIBRARIES}
  ${LCMAPS_RET_POOLIDX_WITHOUT_GSI_LIBRARIES}
  ${LCMAPS_GSS_ASSIST_WITHOUT_GSI_LIBRARIES}
  ${PURGER_LIBRARIES}
  ${ARGUSPEP_LIBRARIES}
  ${RENEWAL_LIBRARIES}
  ${XSLT_LIBRARIED}
  ${EXSLT_LIBRARIES}
  ${GSOAP_PP_LIBRARIES}
  ${GSOAPSSL_PP_LIBRARIES}
  glite_wms_wmproxy_utilities
  glite_wms_wmproxy_eventlogger
  glite_wms_wmproxy_security
  glite_wms_wmproxy_commands
  glite_wms_wmproxy_pipe
)

install(TARGETS 
  glite_wms_wmproxy_server
  DESTINATION 
  ${PREFIX}/${BINDIR}
  )
