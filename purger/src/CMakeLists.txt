
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

link_directories(
  ${WMSUTILS_JOBMAN_EXCEPTION_LIBRARY_DIRS}
  ${WMSUTILS_CLASSADS_LIBRARY_DIRS}
  ${WMS_COMMON_CONF_LIBRARY_DIRS}
  ${WMS_COMMON_LOGGER_LIBRARY_DIRS}
  ${WMS_COMMON_UTIL_LIBRARY_DIRS}
  ${LB_CLIENT_LIBRARY_DIRS}
  ${LB_CLIENTPP_LIBRARY_DIRS}
  ${LB_COMMON_LIBRARY_DIRS}
  ${GLOBUS_OPENSSL_LIBRARY_DIRS}
  ${GLOBUS_COMMON_LIBRARY_DIRS}
  ${GLOBUS_GSS_ASSIST_LIBRARY_DIRS}
  ${CLASSAD_LIBRARY_DIRS}
  ${CARES_LIBRARY_DIRS}
  ${JOBID_LIBRARY_DIRS}
  ${RENEWAL_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS}
  ${LOG4CPP_LIBRARY_DIRS}
)



target_link_libraries(glite-wms-purgeStorage
  glite_wms_purger
  ${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
  ${WMSUTILS_CLASSADS_LIBRARIES}
  ${WMS_COMMON_CONF_LDFLAGS}
  ${WMS_COMMON_LOGGER_LIBRARIES}
  ${WMS_COMMON_UTIL_LIBRARIES}
  ${LB_CLIENT_LIBRARIES}
  ${LB_CLIENTPP_LIBRARIES}
  ${LB_COMMON_LIBRARIES}
  ${GLOBUS_OPENSSL_LIBRARIES}
  ${GLOBUS_COMMON_LIBRARIES}
  ${GLOBUS_GSS_ASSIST_LIBRARIES}
  ${CLASSAD_LIBRARIES}
  ${CARES_LIBRARIES}
  ${JOBID_LIBRARIES}
  ${RENEWAL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${LOG4CPP_LIBRARIES}
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
