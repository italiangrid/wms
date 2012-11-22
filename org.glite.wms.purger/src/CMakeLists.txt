
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PROJECT_SOURCE_DIR}/src
		    ${WMS_COMMON_LOGGER_INCLUDE_DIRS}
		    ${RENEWAL_INCLUDE_DIRS})

add_custom_target(PURGER 
  DEPENDS 
  purger.cpp
  lb_utils.cpp
  ssl_utils.cpp 
)

add_library(glite_wms_purger
  SHARED 
  purger.cpp
  lb_utils.cpp
  ssl_utils.cpp 
  )

set_target_properties(glite_wms_purger PROPERTIES VERSION 0.0.0 SOVERSION 0)

add_executable(glite-wms-purgeStorage glite-wms-purgeStorage.cpp)
target_link_libraries(glite-wms-purgeStorage
  glite_wms_purger
  ${WMSUTILS_JOBMAN_EXCEPTION_LDFLAGS}
  ${WMSUTILS_CLASSADS_LDFLAGS}
  ${WMS_COMMON_CONF_LDFLAGS}
  ${WMS_COMMON_UTIL_LDFLAGS}
  ${WMS_COMMON_LOGGER_LDFLAGS}
  ${LB_CLIENT_LDFLAGS}
  ${LB_CLIENTPP_LDFLAGS}
  ${LB_COMMON_LDFLAGS}
  ${GLOBUS_OPENSSL_LDFLAGS}
  ${GLOBUS_COMMON_LDFLAGS}
  ${GLOBUS_GSS_ASSIST_LDFLAGS}
  ${CLASSAD_LDFLAGS}
  ${CARES_LDFLAGS}
  ${JOBID_LDFLAGS}
  ${RENEWAL_LDFLAGS}
  ${BOOST_FILESYSTEM_LDFLAGS}
  ${BOOST_REGEX_LDFLAGS}
  ${BOOST_OPTIONS_LDFLAGS}
  ${LOG4CPP_LDFLAGS}
  ${BOOST_THREAD_LDFLAGS}
)

install(TARGETS 
  glite_wms_purger
  LIBRARY 
  DESTINATION 
  ${PREFIX}/${LIBDIR})

install(TARGETS 
  glite-wms-purgeStorage
  DESTINATION
  ${PREFIX}/${SBINDIR}
)

add_subdirectory(pkgconfig)

add_custom_command(OUTPUT
  ${PREFIX}/include/glite/wms/purger/purger.h
  COMMAND
  mkdir -p ${PREFIX}/include/glite/wms/purger/
  COMMAND
  cp -f purger.h ${PREFIX}/include/glite/wms/purger/
)

add_custom_target(PURGERHFILE ALL
  DEPENDS
  ${PREFIX}/include/glite/wms/purger/purger.h
)