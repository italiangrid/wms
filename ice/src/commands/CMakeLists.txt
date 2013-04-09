
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
  IceCommandStatusPoller.cpp
  Request_source_purger.cpp
  IceCommandDelegationRenewal.cpp
  IceCommandEventQuery.cpp
  IceCommandLBLogging.cpp
  IceCommandReschedule.cpp
)

add_library(glite_wms_ice_commands
  SHARED 
  IceCommandSubmit.cpp
  IceCommandCancel.cpp
  IceCommandStatusPoller.cpp
  Request_source_purger.cpp
  IceCommandDelegationRenewal.cpp
  IceCommandEventQuery.cpp
  IceCommandLBLogging.cpp
  IceCommandReschedule.cpp
  )

set_target_properties(glite_wms_ice_commands PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_ice_commands
  LIBRARY 
  DESTINATION 
  ${PREFIX}/usr/${LIBDIR}
  )
