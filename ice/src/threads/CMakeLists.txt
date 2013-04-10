
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/src
                    ${MAIN_PROJECT_SOURCE_DIR}
                    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
                    ${MAIN_PROJECT_SOURCE_DIR}/purger/src
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
		    ${Boost_INCLUDE_DIRS}
		    /usr/include)

add_custom_target(ICETHREADS
  DEPENDS
  IceThread.cpp
  EventStatusPoller.cpp
  IceThreadPool.cpp
  DelegationRenewal.cpp	
)

add_library(glite_wms_ice_threads
  SHARED 
  IceThread.cpp
  EventStatusPoller.cpp
  IceThreadPool.cpp
  DelegationRenewal.cpp
  )

set_target_properties(glite_wms_ice_threads PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_ice_threads
  LIBRARY 
  DESTINATION 
  ${PREFIX}/usr/${LIBDIR}
  )
