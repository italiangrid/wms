
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
                    ${CMAKE_CURRENT_BINARY_DIR}/${AUTODIR})



link_directories(${PREFIX}/${LIBDIR})

add_executable(glite-wms-get-configuration get_configuration.cpp)

install(TARGETS glite-wms-get-configuration
        DESTINATION ${PREFIX}/${BINDIR})

#link_directories( ${PROJECT_SOURCE_DIR}/src/configuration 
#                  ${CMAKE_INSTALL_PREFIX}/${LIBDIR})

target_link_libraries(glite-wms-get-configuration 
  glite_wms_conf 
  glite_wms_util 
  ${JOBID_LIBRARIES}
  ${BOOST_REGEX_LIBRARIES}
  ${BOOST_FILESYSTEM_LIBRARIES}
  ${BOOST_DATE_TIME_LIBRARIES}
  ${WMSUTILS_CLASSADS_ALT_LIBRARIES}
  ${WMSUTILS_CLASSADS_LIBRARIES}
  )

#if(DEFINED INSTALL_BUILD)
#  install(TARGETS glite-wms-get-configuration
#    DESTINATION ${INSTALL_BUILD}/${BINDIR})
#endif(DEFINED INSTALL_BUILD)

