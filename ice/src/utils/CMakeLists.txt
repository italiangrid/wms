
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/src
                    ${MAIN_PROJECT_SOURCE_DIR}
                    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
                    ${MAIN_PROJECT_SOURCE_DIR}/purger/src
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
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
  IceLBEventFactory.cpp 
  IceLBEvent.cpp 
  IceLBLogger.cpp 
  Request_jobdir.cpp 
  Request_source_jobdir.cpp 
  RequestParser.cpp 
  Url.cpp
)

add_library(glite_wms_ice_utils
  SHARED 
  IceUtils.cpp
  CreamJob.cpp 
  DNProxyManager.cpp 
  DelegationManager.cpp 
  CEBlackList.cpp 
  CreamProxyMethod.cpp 
  IceConfManager.cpp 
  IceLBContext.cpp 
  IceLBEventFactory.cpp 
  IceLBEvent.cpp 
  IceLBLogger.cpp 
  Request_jobdir.cpp 
  Request_source_jobdir.cpp 
  RequestParser.cpp 
  Url.cpp
  )

set_target_properties(glite_wms_ice_utils PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_ice_utils
  LIBRARY 
  DESTINATION 
  ${PREFIX}/usr/${LIBDIR}
  )
