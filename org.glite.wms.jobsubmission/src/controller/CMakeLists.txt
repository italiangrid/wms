
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PREFIX}/include
		    ${WMSUTILS_CLASSADS_ALT_INCLUDE_DIRS}
		    ${WMSUTILS_CLASSADS_INCLUDE_DIRS}
		    ${PROJECT_SOURCE_DIR}/src
		    ${PROJECT_SOURCE_DIR}/src/)

link_directories(
  ${PREFIX}/${LIBDIR}
  ${CMAKE_SOURCE_DIR}/src/common
  ${CMAKE_BIN_DIR}/src/common
  /usr/${LIBDIR}}/condor
  )

#add_custom_target(LOGMON 
#  DEPENDS
#  EventAd.cpp 
#  Timer.cpp 
#  SizeFile.cpp 
#  AbortedContainer.cpp 
#  JobWrapperOutputParser.cpp 
#  exceptions.cpp 
#  processer/EventFactory.cpp 
#  processer/EventInterface.cpp 
#  processer/EventUnhandled.cpp 
#  processer/EventSubmit.cpp 
#  processer/EventGlobusSubmit.cpp 
#  processer/EventGridSubmit.cpp 
#  processer/EventExecute.cpp 
#  processer/EventTerminated.cpp 
#  processer/EventPostTerminated.cpp 
#  processer/EventAborted.cpp 
#  processer/EventGlobusSubmitFailed.cpp 
#  processer/EventGlobusResourceDown.cpp 
#  processer/EventGridResourceDown.cpp 
#  processer/EventGlobusResourceUp.cpp 
#  processer/EventGridResourceUp.cpp 
#  processer/EventJobHeld.cpp 
#  processer/EventJobReleased.cpp 
#  processer/EventGeneric.cpp 
#  processer/MonitorData.cpp 
#  processer/SubmitReader.cpp 
#  processer/JobResubmitter.cpp
#  )

add_library(glite_wms_jss_controller_wrapper
  SHARED 
  cwrapper.cpp
  )

add_library(glite_wms_jss_controller_adapter
  SHARED 
  SubmitAd.cpp
  SubmitAdExceptions.cpp
  SubmitAdapter.cpp
  )

add_library(glite_wms_jss_controller
  SHARED 
  CondorG.cpp 
  Request.cpp 
  RequestExceptions.cpp 
  JobController.cpp 
  JobControllerExceptions.cpp
  JobControllerFactory.cpp 
  JobControllerProxy.cpp 
  JobControllerReal.cpp 
  JobControllerFake.cpp 
  JobControllerClient.cpp 
  JobControllerClientJD.cpp
  )

set_target_properties(glite_wms_jss_controller_wrapper  PROPERTIES VERSION 0.0.0 SOVERSION 0)
set_target_properties(glite_wms_jss_controller_adapter  PROPERTIES VERSION 0.0.0 SOVERSION 0)
set_target_properties(glite_wms_jss_controller          PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wms_jss_controller
  glite_wms_jss_common
  condorapi
)

install(TARGETS 
  glite_wms_jss_controller
  glite_wms_jss_controller_adapter
  glite_wms_jss_controller_wrapper
  LIBRARY
  DESTINATION 
  ${PREFIX}/${LIBDIR}
  )