add_subdirectory(broker)
add_subdirectory(jobadapter)

include_directories(${CLASSAD_INCLUDE_DIRS}
	 	    ${WMS_COMMON_UTIL_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
                    ${PROJECT_SOURCE_DIR}/src
		    ${CMAKE_INSTALL_PREFIX}/include)

add_custom_target(HELPER DEPENDS 
			         Helper.cpp 
				 exceptions.cpp 
        			 HelperFactory.cpp 
                                 HelperImpl.cpp 
              			 Request.h 
       				 Request.cpp 
	       		         ${PROJECT_SOURCE_DIR}/interface/glite/wms/helper/RequestStateMachine.h 
       				 RequestStateMachine.cpp)


add_library(glite_wms_helper SHARED 
				 Helper.cpp 
				 exceptions.cpp 
        			 HelperFactory.cpp 
                                 HelperImpl.cpp 
       				 Request.cpp 
       				 ${PROJECT_SOURCE_DIR}/interface/glite/wms/helper/RequestStateMachine.h 
       				 RequestStateMachine.cpp)

set_target_properties(glite_wms_helper PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wms_helper 
  ${CLASSAD_LIBRARIES}
  ${JDL_API_CPP_LIBRARIES}
  ${WMS_COMMON_UTIL_LDFLAGS}
  ${WMS_COMMON_UTIL_LIBRARIES}
  ${WMSUTILS_CLASSADS_LIBRARIES}
  ${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
  ${BOOST_FILESYSTEM_LIBRARIES}
)


install(TARGETS glite_wms_helper
        LIBRARY DESTINATION ${PREFIX}/${LIBDIR})

#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
#        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/glite/wms/helper
#        FILES_MATCHING PATTERN "*.h"
#        PATTERN "CMakeFiles" EXCLUDE)

