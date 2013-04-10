
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/src
		    ${MAIN_PROJECT_SOURCE_DIR}
		    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
 		    ${MAIN_PROJECT_SOURCE_DIR}/purger/src
		    ${JOBID_INCLUDE_DIRS}
		    ${PURGER_INCLUDE_DIRS}
		    ${Boost_INCLUDE_DIRS}
		    
		    /usr/include)

add_custom_target(ICE
  DEPENDS	
  Main.cpp
)

add_library(glite_wms_ice_main
  SHARED 
  Main.cpp
  )

set_target_properties(glite_wms_ice_main PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_ice_main
  LIBRARY 
  DESTINATION 
  ${PREFIX}/usr/${LIBDIR}
  )
