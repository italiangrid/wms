
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PREFIX}/include
		    ${WMSUTILS_CLASSADS_ALT_INCLUDE_DIRS}
		    ${WMSUTILS_CLASSADS_INCLUDE_DIRS}
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
		    ${PROJECT_SOURCE_DIR}/src)

link_directories(${PREFIX}/${LIBDIR}
  ${PROJECT_SOURCE_DIR}/src/common
  ${PROJECT_SOURCE_DIR}/src/controller
  ${PROJECT_SOURCE_DIR}/src/logmonitor
  ${WMS_COMMON_CONF_LIBRARY_DIRS}
  ${PURGER_LIBRARY_DIRS}
  ${CONDOR_LIBRARY_DIRS}
  )

add_executable(glite-wms-job_controller
  jobcontroller.cpp
  ControllerLoop.cpp
  exceptions.cpp  
  )


add_executable(glite-wms-log_monitor
  logmonitor.cpp
  MonitorLoop.cpp
  exceptions.cpp
)

target_link_libraries(glite-wms-job_controller
  glite_wms_jss_controller
  glite_wms_jss_controller_adapter
  glite_wms_jss_common
  ${WMS_COMMON_CONF_LIBRARIES}
  ${WMS_COMMON_LOGGER_LIBRARIES}
  ${WMS_COMMON_PROCESS_LIBRARIES}
  ${WMS_COMMON_UTIL_LIBRARIES}
  ${WMSUTILS_CLASSADS_LIBRARIES}
  ${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
  ${JDL_API_CPP_LIBRARIES}
  ${JOBID_LIBRARIES}
  ${LB_CLIENT_LIBRARIES}
  ${LB_CLIENTPP_LIBRARIES}
  ${LB_COMMON_LIBRARIES}
  ${CLASSAD_LIBRARIES}
  ${CARES_LIBRARIES}
  ${PROXYRENEW_LIBRARIES}
  ${PURGER_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CONDOR_LIBRARIES}
  ${CARES_LIBRARIES}
  )


target_link_libraries(glite-wms-log_monitor
  glite_wms_jss_controller
  glite_wms_jss_controller_adapter
  glite_wms_jss_logmonitor
  glite_wms_jss_common
  ${WMS_COMMON_CONF_LIBRARIES}
  ${WMS_COMMON_LOGGER_LIBRARIES}
  ${WMS_COMMON_PROCESS_LIBRARIES}
  ${WMS_COMMON_UTIL_LIBRARIES}
  ${WMSUTILS_CLASSADS_LIBRARIES}
  ${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
  ${JDL_API_CPP_LIBRARIES}
  ${JOBID_LIBRARIES}
  ${LB_CLIENT_LIBRARIES}
  ${LB_CLIENTPP_LIBRARIES}
  ${LB_COMMON_LIBRARIES}
  ${CLASSAD_LIBRARIES}
  ${CARES_LIBRARIES}
  ${PROXYRENEW_LIBRARIES}
  ${PURGER_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CONDOR_LIBRARIES}
  ${CARES_LIBRARIES}
  )

install(TARGETS 
  glite-wms-log_monitor 
  glite-wms-job_controller
  DESTINATION 
  ${PREFIX}/${BINDIR}
)
