

include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
                    ${PROJECT_SOURCE_DIR}/src
		    ${WMS_COMMON_UTIL_INCLUDE_DIRS}
		    ${CMAKE_INSTALL_PREFIX}/include)

add_custom_target(JOBADAPTER DEPENDS 
			           exceptions.cpp
				   JobWrapper.cpp
				   url.cpp
				   JobAdapter.cpp
				   Helper.cpp)

add_library(glite_wms_helper_jobadapter SHARED 
			           exceptions.cpp
				   JobWrapper.cpp
				   url.cpp
				   JobAdapter.cpp
				   Helper.cpp)

set_target_properties(glite_wms_helper_jobadapter PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_helper_jobadapter
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR})
