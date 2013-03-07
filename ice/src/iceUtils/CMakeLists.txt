
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/src
		    #${VOMS_INCLUDE_DIRS}
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
		    #${LOG4CPP_INCLUDE_DIRS}
		    #${CLASSAD_INCLUDE_DIRS}
		    ${Boost_INCLUDE_DIRS}
		    
		    /usr/include)

add_custom_target(ICEUTILS
  DEPENDS	
  IceUtils.cpp
  CreamJob.cpp 
  DNProxyManager.cpp 
  DelegationManager.cpp 
  CEBlackList.cpp 
  CreamProxyMethod.cpp 
  IceConfManager.cpp 
  IceLBContext.cpp 
  iceLBEventFactory.cpp 
  IceLBEvent.cpp 
  iceLBLogger.cpp 
  Request_jobdir.cpp 
  Request_source_jobdir.cpp 
  RequestParser.cpp 
  Url.cpp
)

add_library(glite_wms_iceUtils
  SHARED 
  IceUtils.cpp
  CreamJob.cpp 
  DNProxyManager.cpp 
  DelegationManager.cpp 
  CEBlackList.cpp 
  CreamProxyMethod.cpp 
  IceConfManager.cpp 
  IceLBContext.cpp 
  iceLBEventFactory.cpp 
  IceLBEvent.cpp 
  iceLBLogger.cpp 
  Request_jobdir.cpp 
  Request_source_jobdir.cpp 
  RequestParser.cpp 
  Url.cpp
  )

set_target_properties(glite_wms_iceUtils PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_iceUtils
  LIBRARY 
  DESTINATION 
  ${PREFIX}/${LIBDIR}
  )
