
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PROJECT_SOURCE_DIR}/src)

add_custom_target(LOGGER DEPENDS common.cpp        key_value.cpp  log.cpp
logstream.cpp     manipulators.cpp     wms_log.cpp
container_ts.cpp  logbuf.cpp     logging.cpp  logstream_ts.cpp
manipulators_ts.cpp)

add_library(glite_wms_logger SHARED common.cpp        key_value.cpp  log.cpp
logstream.cpp     manipulators.cpp     wms_log.cpp
container_ts.cpp  logbuf.cpp     logging.cpp  logstream_ts.cpp
manipulators_ts.cpp)

set_target_properties(glite_wms_logger PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wms_logger ${CLASSAD_LIBRARIES})


install(TARGETS glite_wms_logger
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR})


if(DEFINED INSTALL_BUILD_DEV)
install(TARGETS glite_wms_logger
        LIBRARY DESTINATION ${INSTALL_BUILD_DEV}/${LIBDIR})
endif(DEFINED INSTALL_BUILD_DEV)


if(DEFINED INSTALL_BUILD)
install(TARGETS glite_wms_logger
        LIBRARY DESTINATION ${INSTALL_BUILD}/${LIBDIR})
endif(DEFINED INSTALL_BUILD)

