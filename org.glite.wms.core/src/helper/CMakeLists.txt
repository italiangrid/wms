add_subdirectory(broker)
add_subdirectory(jobadapter)
#add_subdirectory(purchaser)
#add_subdirectory(pkgconfig)

include_directories(${CLASSAD_INCLUDE_DIRS}
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
       				 RequestStateMachine.h 
       				 RequestStateMachine.cpp)


add_library(glite_wms_helper SHARED 
				 Helper.cpp 
				 exceptions.cpp 
        			 HelperFactory.cpp 
                                 HelperImpl.cpp 
              			 Request.h 
       				 Request.cpp 
       				 RequestStateMachine.h 
       				 RequestStateMachine.cpp)

set_target_properties(glite_wms_helper PROPERTIES VERSION 0.0.0 SOVERSION 0)

#target_link_libraries(glite_wms_hekper ${CLASSAD_LIBRARIES})


install(TARGETS glite_wms_helper
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/glite/wms/helper
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)

