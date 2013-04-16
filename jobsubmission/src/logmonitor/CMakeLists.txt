
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PREFIX}/usr/include
		    ${WMSUTILS_CLASSADS_ALT_INCLUDE_DIRS}
		    ${WMSUTILS_CLASSADS_INCLUDE_DIRS}
		    ${PROJECT_SOURCE_DIR}/src
		    ${MAIN_PROJECT_SOURCE_DIR}
		    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
		    ${WMS_COMMON_UTIL_INCLUDE_DIRS}
		    ${WMS_COMMON_LOGGER_INCLUDE_DIRS}
		    ${PROJECT_SOURCE_DIR}/src/logmonitor/processer
		    ${LOG4CPP_INCLUDE_DIRS}
		    /usr/include
		    ${CONDOR_INCLUDE_DIRS}
)

link_directories(
  ${PREFIX}/usr/${LIBDIR}
  ${CMAKE_SOURCE_DIR}/src/common
  ${CMAKE_SOURCE_DIR}/src/controller
  ${CONDOR_LIBRARY_DIRS}
  ${MAIN_PROJECT_SOURCE_DIR}/common/src/configuration
  ${MAIN_PROJECT_SOURCE_DIR}/common/src/utilities
  ${MAIN_PROJECT_SOURCE_DIR}/purger/src
  ${LOG4CPP_LIBRARY_DIRS}
  /usr/lib
  /usr/lib64
  )

add_executable(glite-wms-lm-job_status
  jobstatus.cpp
  JobStatusExtractor.cpp
)

target_link_libraries(glite-wms-lm-job_status
  glite_wms_jss_controller
  glite_wms_jss_controller_adapter
  glite_wms_jss_logmonitor
  glite_wms_jss_common
  glite_wms_util
  glite_wms_logger
  glite_wms_purger
  glite_wms_conf
  ${Boost_LIBRARIES}
  ${CARES_LIBRARIES}
  ${CLASSAD_LIBRARIES}
  ${CONDOR_LIBRARIES}
  ${CONDOR_LIBRARIES}
  ${JDL_API_CPP_LIBRARIES}
  ${LB_COMMON_LIBRARIES}
  ${LB_CLIENT_LIBRARIES}
  ${LB_CLIENTPP_LIBRARIES}
  ${WMSUTILS_CLASSADS_LIBRARIES}
  ${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
  ${RENEWAL_LIBRARIES}
  ${JOBID_LIBRARIES}
  ${LOG4CPP_LIBRARIES}
)


add_library(glite_wms_jss_logmonitor
  SHARED 
  CondorMonitor.cpp
  EventAd.cpp 
  Timer.cpp 
  SizeFile.cpp 
  AbortedContainer.cpp 
  JobWrapperOutputParser.cpp 
  exceptions.cpp 
  processer/EventFactory.cpp 
  processer/EventInterface.cpp 
  processer/EventUnhandled.cpp 
  processer/EventSubmit.cpp 
  processer/EventGlobusSubmit.cpp 
  processer/EventGridSubmit.cpp 
  processer/EventExecute.cpp 
  processer/EventTerminated.cpp 
  processer/EventPostTerminated.cpp 
  processer/EventAborted.cpp 
  processer/EventGlobusSubmitFailed.cpp 
  processer/EventGlobusResourceDown.cpp 
  processer/EventGridResourceDown.cpp 
  processer/EventGlobusResourceUp.cpp 
  processer/EventGridResourceUp.cpp 
  processer/EventJobHeld.cpp 
  processer/EventJobReleased.cpp 
  processer/EventGeneric.cpp 
  processer/MonitorData.cpp 
  processer/SubmitReader.cpp 
  processer/JobResubmitter.cpp  
  )

set_target_properties(glite_wms_jss_logmonitor PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wms_jss_logmonitor
  glite_wms_jss_common
  ${CONDOR_LIBRARIES}
)

install(TARGETS glite_wms_jss_logmonitor
  LIBRARY
  DESTINATION 
  ${PREFIX}/usr/${LIBDIR}
  )

install(TARGETS glite-wms-lm-job_status
  DESTINATION
  ${PREFIX}/usr/${LIBEXEC}
)
