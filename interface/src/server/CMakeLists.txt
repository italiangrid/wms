
include_directories(
  ${CLASSAD_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/server
  ${PROJECT_SOURCE_DIR}/src/server/autogen
  ${MAIN_PROJECT_SOURCE_DIR}
  ${MAIN_PROJECT_SOURCE_DIR}/common/interface
  ${WMSUTILS_JOBMAN_EXCEPTION_INCLUDE_DIRS}
  ${WMSUTILS_CLASSADS_INCLUDE_DIRS}
  ${JDL_API_CPP_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${GLOBUS_GSSAPI_INCLUDE_DIRS}
  ${ARGUSPEP_INCLUDE_DIRS}
  ${LCMAPS_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${LOG4CPP_INCLUDE_DIRS}
  /usr/include
  )

add_custom_command(
  OUTPUT 
  ${PROJECT_SOURCE_DIR}/src/server/autogen/soapC.cpp 
  ${PROJECT_SOURCE_DIR}/src/server/autogen/soapServer.cpp
  ${PROJECT_SOURCE_DIR}/src/server/autogen/soapClient.cpp
  ${PROJECT_SOURCE_DIR}/src/server/autogen/wm.h 
  COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/src/server/autogen
  COMMAND
  wsdl2h -o ${PROJECT_SOURCE_DIR}/src/server/autogen/wm.h 
         -t ${PROJECT_SOURCE_DIR}/src/server/wmptypemap.dat 
         ${PROJECT_SOURCE_DIR}/src/server/WMProxy.wsdl
  COMMAND 
  soapcpp2 -d${PROJECT_SOURCE_DIR}/src/server/autogen
-I/usr/include:/usr/import:/usr/share/gsoap/import:/usr/include/gsoap
${PROJECT_SOURCE_DIR}/src/server/autogen/wm.h
  )

add_custom_target(GENSTUBS2 ALL DEPENDS 
  ${PROJECT_SOURCE_DIR}/src/server/autogen/soapC.cpp 
  ${PROJECT_SOURCE_DIR}/src/server/autogen/soapServer.cpp
  ${PROJECT_SOURCE_DIR}/src/server/autogen/soapClient.cpp
  ${PROJECT_SOURCE_DIR}/src/server/autogen/wm.h
  )

link_directories(${PREFIX}/usr/${LIBDIR}
  ${PROJECT_SOURCE_DIR}/src/utilities
  ${PROJECT_SOURCE_DIR}/src/security
  ${PROJECT_SOURCE_DIR}/src/eventlogger
  ${PROJECT_SOURCE_DIR}/src/dirmanager
  ${PROJECT_SOURCE_DIR}/src/commands
  ${WMS_COMMON_LOGGER_LIBRARY_DIRS}
  ${WMSUTILS_CLASSADS_LIBRARY_DIRS}
  ${PURGER_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS}
  ${WMSUTILS_JOBMAN_EXCEPTION_LIBRARY_DIRS}
  ${LOG4CPP_LIBRARY_DIRS} 
  /usr/lib
  /usr/lib64
  )

add_executable(glite_wms_wmproxy_server
  autogen/wm.h
  autogen/soapC.cpp
  autogen/soapServer.cpp
  autogen/soapClient.cpp
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
  glite_wms_logger
  glite_wms_conf
  glite_wms_util
  glite_wms_quota
  glite_wms_purger
  ${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
  ${WMSUTILS_CLASSADS_LIBRARIES}
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
  ${ARGUSPEP_LIBRARIES}
  ${RENEWAL_LIBRARIES}
  ${XSLT_LIBRARIED}
  ${EXSLT_LIBRARIES}
  ${GSOAP_PP_LIBRARIES}
  ${GSOAPSSL_PP_LIBRARIES}
  ${LOG4CPP_LIBRARIES}
  glite_wms_wmproxy_utilities
  glite_wms_wmproxy_eventlogger
  glite_wms_wmproxy_security
  glite_wms_wmproxy_commands
  glite_wms_wmproxy_pipe
)

install(TARGETS 
  glite_wms_wmproxy_server
  DESTINATION 
  ${PREFIX}/usr/${BINDIR}
  )
