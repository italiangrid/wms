include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
                    ${PROJECT_SOURCE_DIR}/src/purchaser
		    ${PROJECT_SOURCE_DIR}/src
		    ${CMAKE_INSTALL_PREFIX}/include)

add_custom_target(ISM_PURCHASER DEPENDS common.cpp schema_utils.cpp
ism-ii-purchaser.cpp)

add_library(glite_wms_ism_ii_purchaser SHARED common.cpp schema_utils.cpp
ism-ii-purchaser.cpp)

set_target_properties(glite_wms_ism_ii_purchaser  PROPERTIES VERSION 0.0.0 SOVERSION 0)



install(TARGETS glite_wms_ism_ii_purchaser 
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR})

