
add_subdirectory(ism)
add_subdirectory(helper)


include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
		    ${MAIN_PROJECT_SOURCE_DIR}
		    ${PREFIX}/usr/include
		    ${PROJECT_SOURCE_DIR}/src		    
${Boost_INCLUDE_DIRS}
${CARES_INCLUDE_DIRS}
${GSOAP_INCLUDE_DIRS}
${GSOAP_PLUSPLUS_INCLUDE_DIRS}
${GSOAP_SSL_INCLUDE_DIRS}
${GSOAP_SSL_PLUSPLUS_INCLUDE_DIRS}
${GSOAP_PLUGIN_INCLUDE_DIRS}
${CLASSAD_INCLUDE_DIRS}
${WMSUTILS_CLASSADS_INCLUDE_DIRS}
${WMSUTILS_JOBMAN_EXCEPTION_INCLUDE_DIRS}
${JDL_API_CPP_INCLUDE_DIRS}
${WMS_COMMON_CONF_INCLUDE_DIRS}
${WMS_COMMON_LOGGER_INCLUDE_DIRS}
${WMS_COMMON_PROCESS_INCLUDE_DIRS}
${WMS_COMMON_UTIL_INCLUDE_DIRS}
${WMS_COMMON_QUOTA_INCLUDE_DIRS}
${LB_COMMON_INCLUDE_DIRS}
${LB_CLIENT_INCLUDE_DIRS}
${LB_CLIENTPP_INCLUDE_DIRS}
${RENEWAL_INCLUDE_DIRS}
${PURGER_INCLUDE_DIRS}
${JOBID_INCLUDE_DIRS}
${GLOBUS_GSSAPI_INCLUDE_DIRS}
${LOG4CPP_INCLUDE_DIRS}
)

link_directories(${PREFIX}/usr/${LIBDIR}
${MAIN_PROJECT_SOURCE_DIR}/common/src/utilities
${MAIN_PROJECT_SOURCE_DIR}/common/src/logger
${MAIN_PROJECT_SOURCE_DIR}/common/src/configuration
${MAIN_PROJECT_SOURCE_DIR}/purger/src
${Boost_LIBRARY_DIRS}
${CARES_LIBRARY_DIRS}
${GSOAP_LIBRARY_DIRS}
${GSOAP_PLUSPLUS_LIBRARY_DIRS}
${GSOAP_SSL_LIBRARY_DIRS}
${GSOAP_SSL_PLUSPLUS_LIBRARY_DIRS}
${GSOAP_PLUGIN_LIBRARY_DIRS}
${CLASSAD_LIBRARY_DIRS}
${WMSUTILS_CLASSADS_LIBRARY_DIRS}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARY_DIRS}
${JDL_API_CPP_LIBRARY_DIRS}
${LB_COMMON_LIBRARY_DIRS}
${LB_CLIENT_LIBRARY_DIRS}
${LB_CLIENTPP_LIBRARY_DIRS}
${RENEWAL_LIBRARY_DIRS}
${PURGER_LIBRARY_DIRS}
${JOBID_LIBRARY_DIRS}
${GLOBUS_GSSAPI_LIBRARY_DIRS}
${LOG4CPP_LIBRARY_DIRS}
)

add_executable(glite-wms-query-job-state-transitions
  glite-wms-query-job-state-transitions.cpp
)

add_executable(glite-wms-workload_manager 
  main.cpp 
  ism_utils.cpp 
  bulkmm_utils.h 
  bulkmm_utils.cpp 
  listmatch.h 
  listmatch.cpp 
  plan.h 
  plan.cpp 
  submit_request.h 
  submit_request.cpp 
  cancel_request.h 
  cancel_request.cpp 
  match_request.h 
  match_request.cpp 
  events.h 
  events.cpp 
  dispatcher_utils.h 
  dispatcher_utils.cpp 
  wm_real.h 
  wm_real.cpp 
  signal_handling.h 
  signal_handling.cpp 
  match_utils.h 
  match_utils.cpp 
  recovery.h 
  recovery.cpp 
  lb_utils.h 
  lb_utils.cpp 
  dynamic_library.h 
  dynamic_library.cpp 
  submission_utils.h 
  submission_utils.cpp 
  replanner.cpp)

install(TARGETS 
  glite-wms-workload_manager
  glite-wms-query-job-state-transitions
  DESTINATION 
  ${PREFIX}/usr/${BINDIR}
)

target_link_libraries(glite-wms-query-job-state-transitions
glite_wms_ism
glite_wms_ism_ii_purchaser
glite_wms_helper_broker_ism
glite_wms_helper_jobadapter
glite_wms_helper
glite_wms_conf
glite_wms_util
glite_wms_logger
${Boost_LIBRARIES}
${CARES_LIBRARIES}
${GSOAP_LIBRARIES}
${GSOAP_PLUSPLUS_LIBRARIES}
${GSOAP_SSL_LIBRARIES}
${GSOAP_SSL_PLUSPLUS_LIBRARIES}
${GSOAP_PLUGIN_LIBRARIES}
${CLASSAD_LIBRARIES}
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
${JDL_API_CPP_LIBRARIES}
${LB_COMMON_LIBRARIES}
${LB_CLIENT_LIBRARIES}
${LB_CLIENTPP_LIBRARIES}
${RENEWAL_LIBRARIES}
${PURGER_LIBRARIES}
${JOBID_LIBRARIES}
${GLOBUS_GSSAPI_LIBRARIES}
${LOG4CPP_LIBRARIES}
)


target_link_libraries(glite-wms-workload_manager
glite_wms_ism
glite_wms_ism_ii_purchaser
glite_wms_helper_broker_ism
glite_wms_helper_jobadapter
glite_wms_helper
glite_wms_conf
glite_wms_util
glite_wms_logger
glite_wms_purger
expat
${Boost_LIBRARIES}
${CARES_LIBRARIES}
${GSOAP_LIBRARIES}
${GSOAP_PLUSPLUS_LIBRARIES}
${GSOAP_SSL_LIBRARIES}
${GSOAP_SSL_PLUSPLUS_LIBRARIES}
${GSOAP_PLUGIN_LIBRARIES}
${CLASSAD_LIBRARIES}
${WMSUTILS_CLASSADS_LIBRARIES}
${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
${JDL_API_CPP_LIBRARIES}
${LB_COMMON_LIBRARIES}
${LB_CLIENT_LIBRARIES}
${LB_CLIENTPP_LIBRARIES}
${RENEWAL_LIBRARIES}
${PURGER_LIBRARIES}
${JOBID_LIBRARIES}
${GLOBUS_GSSAPI_LIBRARIES}
${LOG4CPP_LIBRARIES}
)

install(FILES
  glite-wms-stats.py
  DESTINATION ${PREFIX}/usr/${BINDIR}
  RENAME glite-wms-stats
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )

#add_custom_target(SYMLINKS ALL DEPENDS
#  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-stats.py
#  COMMAND
#  mkdir -p ${PREFIX}/usr/${BINDIR}
#  COMMAND
#  cp -f ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-stats.py ${PREFIX}/usr/${BINDIR}/glite-wms-stats
#  COMMAND
#  chmod 755 ${PREFIX}/usr/${BINDIR}/glite-wms-stats
#)
