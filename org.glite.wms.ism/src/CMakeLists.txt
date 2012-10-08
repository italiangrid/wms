add_subdirectory(purchaser)

include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
                    ${PROJECT_SOURCE_DIR}/src
		    ${CMAKE_INSTALL_PREFIX}/include)

add_custom_target(ISM DEPENDS ism.cpp)

add_library(glite_wms_ism SHARED ism.cpp)

set_target_properties(glite_wms_ism PROPERTIES VERSION 0.0.0 SOVERSION 0)

#target_link_libraries(glite_wms_ism ${CLASSAD_LIBRARIES})


install(TARGETS glite_wms_ism
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR})

