
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/src
                    ${MAIN_PROJECT_SOURCE_DIR}
                    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
                    ${MAIN_PROJECT_SOURCE_DIR}/purger/src
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
		    ${Boost_INCLUDE_DIRS}
		    /usr/include)

add_custom_target(ICECOMMANDS 
  DEPENDS 
  IceCommandSubmit.cpp
  IceCommandCancel.cpp
  iceCommandStatusPoller.cpp
  Request_source_purger.cpp
  IceCommandDelegationRenewal.cpp
  iceCommandEventQuery.cpp
  iceCommandLBLogging.cpp
  iceCommandReschedule.cpp
)

add_library(glite_wms_IceCommands
  SHARED 
  IceCommandSubmit.cpp
  IceCommandCancel.cpp
  iceCommandStatusPoller.cpp
  Request_source_purger.cpp
  IceCommandDelegationRenewal.cpp
  iceCommandEventQuery.cpp
  iceCommandLBLogging.cpp
  iceCommandReschedule.cpp
  )

set_target_properties(glite_wms_IceCommands PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_IceCommands
  LIBRARY 
  DESTINATION 
  ${PREFIX}/usr/${LIBDIR}
  )
