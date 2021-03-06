add_subdirectory(commands)
add_subdirectory(utils)
add_subdirectory(threads)
add_subdirectory(db)
add_subdirectory(main)

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
${GSOAP_PLUGIN_INCLUDE_DIRS}
${MAIN_PROJECT_SOURCE_DIR}
${MAIN_PROJECT_SOURCE_DIR}/common/interface
${WMSUTILS_CLASSADS_INCLUDE_DIRS}
${WMSUTILS_JOBMAN_EXCEPTION_INCLUDE_DIRS}
/usr/include
)

link_directories(${PREFIX}/usr/${LIBDIR}
  ${PROJECT_SOURCE_DIR}/src/utilities
  ${PROJECT_SOURCE_DIR}/src/security
  ${PROJECT_SOURCE_DIR}/src/eventlogger
  ${PROJECT_SOURCE_DIR}/src/dirmanager
  ${PROJECT_SOURCE_DIR}/src/commands
  ${MAIN_PROJECT_SOURCE_DIR}/common/src/configuration
  ${MAIN_PROJECT_SOURCE_DIR}/common/src/utilities
  ${MAIN_PROJECT_SOURCE_DIR}/common/src/logger
  ${MAIN_PROJECT_SOURCE_DIR}/common/src/process
  ${MAIN_PROJECT_SOURCE_DIR}/purger/src
  ${Boost_LIBRARY_DIRS}
  ${CLASSAD_LIBRARY_DIRS}
  ${JDL_API_CPP_LIBRARY_DIRS}
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
  ${GSOAP_PLUGIN_LIBRARY_DIRS}
  ${WMSUTILS_CLASSADS_LIBRARY_DIRS}
  ${WMSUTILS_JOBMAN_EXCEPTION_LIBRARY_DIRS}
  /usr/lib
  /usr/lib64
  )
  
add_executable(glite-wms-ice 			glite-wms-ice.cpp)
add_executable(glite-wms-ice-query-db 		query-db.cpp)
add_executable(glite-wms-ice-query-db-history 	query-db-history.cpp)
add_executable(glite-wms-ice-query-stats 	query-stats.cpp)
add_executable(glite-wms-ice-putfl 		putfl.cpp)
add_executable(glite-wms-ice-putfl-reschedule 	putfl-reschedule.cpp)
add_executable(glite-wms-ice-putfl-cancel 	putfl-cancel.cpp)
add_executable(glite-wms-ice-safe 		glite-wms-ice-safe.cpp)
add_executable(glite-wms-ice-rm 		glite-wms-ice-rm.cpp)
add_executable(glite-wms-ice-db-rm 		glite-wms-ice-db-rm.cpp)
add_executable(glite-wms-ice-proxy-renew 	glite-wms-ice-proxy-renew.cpp)

target_link_libraries(glite-wms-ice
glite_wms_ice_commands
glite_wms_ice_threads
glite_wms_ice_utils
glite_wms_ice_db
glite_wms_ice_main
glite_wms_logger
glite_wms_conf
glite_wms_util
glite_wms_process
glite_wms_purger
sqlite3
${GSOAP_PLUGIN_LIBRARIES}
${CLASSAD_LIBRARIES}
${JDL_API_CPP_LIBRARIES}
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
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
)

target_link_libraries(glite-wms-ice-query-db
glite_wms_ice_commands 
glite_wms_ice_threads 
glite_wms_ice_utils 
glite_wms_ice_db
glite_wms_ice_main
glite_wms_logger
glite_wms_conf
glite_wms_util
glite_wms_purger
glite_wms_process
sqlite3
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
${GSOAP_PLUGIN_LIBRARIES}
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

target_link_libraries(glite-wms-ice-query-db-history
glite_wms_ice_commands 
glite_wms_ice_threads
glite_wms_ice_utils 
glite_wms_ice_db
glite_wms_ice_main
glite_wms_logger
glite_wms_conf
glite_wms_util
glite_wms_purger
glite_wms_process
sqlite3
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
${GSOAP_PLUGIN_LIBRARIES}
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


target_link_libraries(glite-wms-ice-query-stats 
glite_wms_ice_commands 
glite_wms_ice_threads 
glite_wms_ice_utils 
glite_wms_ice_db
glite_wms_ice_main
glite_wms_logger
glite_wms_conf
glite_wms_util
glite_wms_purger
glite_wms_process
sqlite3 ${GSOAP_PLUGIN_LIBRARIES}
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(glite-wms-ice-putfl 
glite_wms_ice_commands 
glite_wms_ice_threads 
glite_wms_ice_utils 
glite_wms_ice_db
glite_wms_ice_main
glite_wms_logger
glite_wms_conf
glite_wms_util
glite_wms_purger
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
glite_wms_process sqlite3 ${GSOAP_PLUGIN_LIBRARIES} ${CLASSAD_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES} ${JDL_API_CPP_LIBRARIES}
${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES} ${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES}
${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(glite-wms-ice-putfl-reschedule
glite_wms_ice_commands 
glite_wms_ice_threads 
glite_wms_ice_utils 
glite_wms_ice_db
glite_wms_ice_main
glite_wms_logger
glite_wms_conf
glite_wms_purger
glite_wms_util
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
glite_wms_process sqlite3 ${GSOAP_PLUGIN_LIBRARIES}
${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(glite-wms-ice-putfl-cancel
glite_wms_ice_commands 
glite_wms_ice_threads 
glite_wms_ice_utils 
glite_wms_ice_db
glite_wms_ice_main
glite_wms_logger
glite_wms_conf
glite_wms_purger
glite_wms_util
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
glite_wms_process sqlite3 ${GSOAP_PLUGIN_LIBRARIES}
${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(glite-wms-ice-safe
glite_wms_ice_commands 
glite_wms_ice_threads 
glite_wms_ice_utils 
glite_wms_ice_db
glite_wms_ice_main
glite_wms_logger
glite_wms_conf
glite_wms_purger
glite_wms_util
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
glite_wms_process sqlite3 ${GSOAP_PLUGIN_LIBRARIES}
${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(glite-wms-ice-rm 
glite_wms_ice_commands 
glite_wms_ice_threads 
glite_wms_ice_utils 
glite_wms_ice_db
glite_wms_ice_main
glite_wms_logger
glite_wms_purger
glite_wms_conf
glite_wms_util
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
glite_wms_process sqlite3 ${GSOAP_PLUGIN_LIBRARIES} ${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES}
${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(glite-wms-ice-db-rm
glite_wms_ice_commands 
glite_wms_ice_threads 
glite_wms_ice_utils 
glite_wms_ice_db
glite_wms_ice_main
glite_wms_logger
glite_wms_purger
glite_wms_conf
glite_wms_util
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
glite_wms_process sqlite3 ${GSOAP_PLUGIN_LIBRARIES} ${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES}
${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

target_link_libraries(glite-wms-ice-proxy-renew
glite_wms_ice_commands 
glite_wms_ice_threads 
glite_wms_ice_utils 
glite_wms_ice_db
glite_wms_ice_main
glite_wms_logger
glite_wms_purger
glite_wms_conf
glite_wms_util
glite_wms_process
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
sqlite3 ${GSOAP_PLUGIN_LIBRARIES}
${CLASSAD_LIBRARIES} ${JDL_API_CPP_LIBRARIES} ${WMS_COMMON_CONF_LIBRARIES} ${WMS_COMMON_UTIL_LIBRARIES} ${PURGER_LIBRARIES} ${LB_COMMON_LIBRARIES} ${LB_CLIENTPP_LIBRARIES}
${JDL_API_CPP_LIBRARIES} ${JOBID_LIBRARIES} ${GRIDSITE_OPENSSL_LIBRARIES} ${VOMS_LIBRARIES} ${RENEWAL_LIBRARIES} ${GSOAP_PP_LIBRARIES}
${GSOAPSSL_PP_LIBRARIES} ${LIBXML_LIBRARIES} ${CREAM_CLIENT_UTIL_LIBRARIES} ${CREAM_CLIENT_SOAP_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS 
  glite-wms-ice
  glite-wms-ice-safe
  glite-wms-ice-proxy-renew
  glite-wms-ice-db-rm
  glite-wms-ice-rm
  glite-wms-ice-putfl-cancel
  glite-wms-ice-putfl-reschedule
  glite-wms-ice-putfl
  glite-wms-ice-query-db
  glite-wms-ice-query-db-history
  glite-wms-ice-query-stats
  DESTINATION 
  ${PREFIX}/usr/${BINDIR}
  )

