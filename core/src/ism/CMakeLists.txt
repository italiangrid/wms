add_subdirectory(purchaser)

include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PROJECT_SOURCE_DIR}/src
		    ${MAIN_PROJECT_SOURCE_DIR}
		    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
		    ${WMS_COMMON_LOGGER_INCLUDE_DIRS}
		    /usr/include)

add_custom_target(ISM 
  DEPENDS 
  ism.cpp
)

add_library(glite_wms_ism
  SHARED 
  ism.cpp
  )

set_target_properties(glite_wms_ism PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wms_ism
  ${WMS_COMMON_CONF_LDFLAGS}
  ${WMS_COMMON_LOGGER_LDFLAGS}
  ${BOOST_REGEX_LDFLAGS}
  ${BOOST_THREAD_LDFLAGS}
  ${BOOST_DATE_TIME_LDFLAGS}
  ${BOOST_FILESYSTEM_LDFLAGS}
  ${CLASSAD_LDFLAGS}
  )


install(TARGETS glite_wms_ism
  LIBRARY 
  DESTINATION 
  ${PREFIX}/${LIBDIR}
  )
