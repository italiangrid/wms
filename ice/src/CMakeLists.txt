add_subdirectory(iceCommands)
add_subdirectory(iceUtils)
add_subdirectory(iceThreads)
add_subdirectory(iceDb)
add_subdirectory(ice)

include_directories(
${PROJECT_SOURCE_DIR}/src
${CLASSAD_INCLUDE_DIRS}
${JDL_API_CPP_INCLUDE_DIRS}
${WMS_COMMON_CONF_INCLUDE_DIRS}
${WMS_COMMON_UTIL_INCLUDE_DIRS}
${PURGER_INCLUDE_DIRS}
${LB_COMMON_INCLUDE_DIRS}
${LB_CLIENTPP_INCLUDE_DIRS}
${JDL_API_CPP_INCLUDE_DIRS}
${JOBID_INCLUDE_DIRS}
${GRIDSITE_OPENSSL_INCLUDE_DIRS}
${VOMS_INCLUDE_DIRS}
${RENEWAL_INCLUDE_DIRS}
${GSOAP_PP_INCLUDE_DIRS}
${GSOAPSSL_PP_INCLUDE_DIRS}
${LIBXML_INCLUDE_DIRS}
${CREAM_CLIENT_UTIL_INCLUDE_DIRS}
${CREAM_CLIENT_SOAP_INCLUDE_DIRS}
${GSOAP_PLUGINPP_INCLUDE_DIRS}
)

link_directories(${PREFIX}/${LIBDIR}
  ${PROJECT_SOURCE_DIR}/src/utilities
  ${PROJECT_SOURCE_DIR}/src/security
  ${PROJECT_SOURCE_DIR}/src/eventlogger
  ${PROJECT_SOURCE_DIR}/src/dirmanager
  ${PROJECT_SOURCE_DIR}/src/commands
  ${WMS_COMMON_LOGGER_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS}
  ${CLASSAD_LIBRARY_DIRS}
  ${JDL_API_CPP_LIBRARY_DIRS}
  ${WMS_COMMON_CONF_LIBRARY_DIRS}
  ${WMS_COMMON_UTIL_LIBRARY_DIRS}
  ${PURGER_LIBRARY_DIRS}
  ${LB_COMMON_LIBRARY_DIRS}
  ${LB_CLIENTPP_LIBRARY_DIRS}
  ${JDL_API_CPP_LIBRARY_DIRS}
  ${JOBID_LIBRARY_DIRS}
  ${GRIDSITE_OPENSSL_LIBRARY_DIRS}
  ${VOMS_LIBRARY_DIRS}
  ${RENEWAL_LIBRARY_DIRS}
  ${GSOAP_PP_LIBRARY_DIRS}
  ${GSOAPSSL_PP_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIRS}
  ${CREAM_CLIENT_UTIL_LIBRARY_DIRS}
  ${CREAM_CLIENT_SOAP_LIBRARY_DIRS}  
  ${GSOAP_PLUGINPP_LIBRARY_DIRS}
  )
  
add_executable(glite-wms-ice
 glite-wms-ice.cpp
)
add_executable(queryDb queryDb.cpp)
add_executable(queryStats queryStats.cpp)
add_executable(putFL putFL.cpp)
add_executable(putFL_reschedule putFL_reschedule.cpp)
add_executable(putFL_cancel putFL_cancel.cpp)
add_executable(glite-wms-ice-safe glite-wms-ice-safe.cpp)
add_executable(glite-wms-ice-rm glite-wms-ice-rm.cpp)
add_executable(glite-wms-ice-db-rm glite-wms-ice-db-rm.cpp)
add_executable(glite-wms-ice-proxy-renew glite-wms-ice-proxy-renew.cpp)

target_link_libraries(glite-wms-ice
glite_wms_iceCommands
glite_wms_iceThreads
glite_wms_iceUtils
glite_wms_iceDb
glite_wms_ice
sqlite3
${GSOAP_PLUGINPP_LIBRARIES}
${CLASSAD_LIBRARIES}
${JDL_API_CPP_LIBRARIES}
${WMS_COMMON_CONF_LIBRARIES}
${WMS_COMMON_UTIL_LIBRARIES}
${PURGER_LIBRARIES}
${LB_COMMON_LIBRARIES}
${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES}
${JOBID_LIBRARIES}
${GRIDSITE_OPENSSL_LIBRARIES}
${VOMS_LIBRARIES}
${RENEWAL_LIBRARIES}
${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES}
${LIBXML_LIBRARIES}
${CREAM_CLIENT_UTIL_LIBRARIES}
${CREAM_CLIENT_SOAP_LIBRARIES}
${Boost_LIBRARIES}
)

target_link_libraries(queryDb glite_wms_iceCommands glite_wms_iceThreads glite_wms_iceUtils glite_wms_iceDb glite_wms_ice sqlite3 ${GSOAP_PLUGINPP_LIBRARIES}
${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(queryStats glite_wms_iceCommands glite_wms_iceThreads glite_wms_iceUtils glite_wms_iceDb glite_wms_ice sqlite3 ${GSOAP_PLUGINPP_LIBRARIES}
${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(putFL glite_wms_iceCommands glite_wms_iceThreads glite_wms_iceUtils glite_wms_iceDb glite_wms_ice sqlite3 ${GSOAP_PLUGINPP_LIBRARIES} ${CLASSAD_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES} ${JDL_API_CPP_LIBRARIES}
${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES} ${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES}
${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(putFL_reschedule glite_wms_iceCommands glite_wms_iceThreads glite_wms_iceUtils glite_wms_iceDb glite_wms_ice sqlite3 ${GSOAP_PLUGINPP_LIBRARIES}
${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(putFL_cancel glite_wms_iceCommands glite_wms_iceThreads glite_wms_iceUtils glite_wms_iceDb glite_wms_ice sqlite3 ${GSOAP_PLUGINPP_LIBRARIES}
${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(glite-wms-ice-safe glite_wms_iceCommands glite_wms_iceThreads glite_wms_iceUtils glite_wms_iceDb glite_wms_ice sqlite3 ${GSOAP_PLUGINPP_LIBRARIES}
${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(glite-wms-ice-rm glite_wms_iceCommands glite_wms_iceThreads glite_wms_iceUtils glite_wms_iceDb glite_wms_ice sqlite3 ${GSOAP_PLUGINPP_LIBRARIES} ${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES}
${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(glite-wms-ice-db-rm glite_wms_iceCommands glite_wms_iceThreads glite_wms_iceUtils glite_wms_iceDb glite_wms_ice sqlite3 ${GSOAP_PLUGINPP_LIBRARIES} ${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES}
${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(glite-wms-ice-proxy-renew glite_wms_iceCommands glite_wms_iceThreads glite_wms_iceUtils glite_wms_iceDb glite_wms_ice sqlite3 ${GSOAP_PLUGINPP_LIBRARIES}
${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS 
  glite-wms-ice
  glite-wms-ice-safe
  glite-wms-ice-proxy-renew
  glite-wms-ice-db-rm
  glite-wms-ice-rm
  putFL_cancel
  putFL_reschedule
  putFL
  queryDb
  queryStats
  DESTINATION 
  ${PREFIX}/${BINDIR}
  )
