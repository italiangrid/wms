
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/src
		    #${VOMS_INCLUDE_DIRS}
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
		    #${LOG4CPP_INCLUDE_DIRS}
		    #${CLASSAD_INCLUDE_DIRS}
		    ${Boost_INCLUDE_DIRS}
		    
		    /usr/include)

add_custom_target(ICETHREADS
  DEPENDS
  iceThread.cpp
  eventStatusPoller.cpp
  iceThreadPool.cpp
  proxyRenewal.cpp	
)

add_library(glite_wms_iceThreads
  SHARED 
  iceThread.cpp
  eventStatusPoller.cpp
  iceThreadPool.cpp
  proxyRenewal.cpp
  )

set_target_properties(glite_wms_iceThreads PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_iceThreads
  LIBRARY 
  DESTINATION 
  ${PREFIX}/${LIBDIR}
  )
