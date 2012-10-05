add_subdirectory(pkgconfig)

include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
                    ${CMAKE_CURRENT_BINARY_DIR}/${AUTODIR})

add_custom_target(ALL DEPENDS Exception.cpp)

add_library(glite_wmsutils_exception SHARED Exception.cpp)

set_target_properties(glite_wmsutils_exception PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wmsutils_exception ${CLASSAD_LIBRARIES})


install(TARGETS glite_wmsutils_exception
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR})
