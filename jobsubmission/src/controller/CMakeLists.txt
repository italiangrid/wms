
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PREFIX}/include
		    ${WMSUTILS_CLASSADS_ALT_INCLUDE_DIRS}
		    ${WMSUTILS_CLASSADS_INCLUDE_DIRS}
		    ${PROJECT_SOURCE_DIR}/src
		    ${MAIN_PROJECT_SOURCE_DIR}
		    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
		    ${WMS_COMMON_UTIL_INCLUDE_DIRS}
		    ${WMS_COMMON_LOGGER_INCLUDE_DIRS}
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
		    ${PROJECT_SOURCE_DIR}/src/)

link_directories(
  ${PREFIX}/${LIBDIR}
  ${CMAKE_SOURCE_DIR}/src/common
  ${CMAKE_BIN_DIR}/src/common
  ${CONDOR_LIBRARY_DIRS}
  )

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
  ${CONDOR_LIBRARIES}
)

install(TARGETS 
  glite_wms_jss_controller
  glite_wms_jss_controller_adapter
  glite_wms_jss_controller_wrapper
  LIBRARY
  DESTINATION 
  ${PREFIX}/${LIBDIR}
  )
