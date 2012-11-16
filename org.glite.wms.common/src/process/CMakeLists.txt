
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PROJECT_SOURCE_DIR}/src)

add_custom_target(PROCESS 
  DEPENDS 
  process.cpp 
  subprocess.cpp 
  user.cpp)

add_library(glite_wms_process 
  SHARED 
  process.cpp 
  subprocess.cpp 
  user.cpp)

set_target_properties(glite_wms_process PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wms_process ${CLASSAD_LIBRARIES})

install(TARGETS glite_wms_process
        LIBRARY 
	DESTINATION 
	${CMAKE_INSTALL_PREFIX}/${LIBDIR})

#if(DEFINED INSTALL_BUILD_DEV)
#install(TARGETS glite_wms_process
#        LIBRARY DESTINATION ${INSTALL_BUILD_DEV}/${LIBDIR})
#endif(DEFINED INSTALL_BUILD_DEV)


#if(DEFINED INSTALL_BUILD)
#install(TARGETS glite_wms_process
#        LIBRARY DESTINATION ${INSTALL_BUILD}/${LIBDIR})
#endif(DEFINED INSTALL_BUILD)

