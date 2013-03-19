
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PROJECT_SOURCE_DIR}/src
		    ${MAIN_PROJECT_SOURCE_DIR}
		    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
		    /usr/include)

add_custom_target(JOBADAPTER 
  DEPENDS 
  exceptions.cpp
  JobWrapper.cpp
  url.cpp
  JobAdapter.cpp
  Helper.cpp
)

add_library(glite_wms_helper_jobadapter
  SHARED 
  exceptions.cpp
  JobWrapper.cpp
  url.cpp
  JobAdapter.cpp
  Helper.cpp
  )

set_target_properties(glite_wms_helper_jobadapter PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wms_helper_jobadapter
  ${JDL_API_CPP_LDFLAGS}
  ${WMS_COMMON_CONF_LDFLAGS}
  ${WMS_COMMON_UTIL_LDFLAGS}
  ${CLASSAD_LDFLAGS}
  ${BOOST_REGEX_LDFLAGS}
  ${BOOST_DATE_TIME_LDFLAGS}
  ${BOOST_FILESYSTEM_LDFLASGS}
  ${BOOST_THREAD_LDFLAGS}
  ${WMSUTILS_CLASSADS_LDFLAGS}
  ${WMSUTILS_JOBMAN_EXCEPTION_LDFLAGS}
  )


install(TARGETS glite_wms_helper_jobadapter
  LIBRARY 
  DESTINATION 
  ${PREFIX}/stage/usr/${LIBDIR}
  )
