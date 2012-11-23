
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PREFIX}/include
		    ${WMSUTILS_CLASSADS_ALT_INCLUDE_DIRS}
		    ${WMSUTILS_CLASSADS_INCLUDE_DIRS}
		    ${PROJECT_SOURCE_DIR}/src)

link_directories(${PREFIX}/${LIBDIR})

add_executable(glite-wms-workload_manager 
  main.cpp 
  ism_utils.cpp 
  bulkmm_utils.h 
  bulkmm_utils.cpp 
  listmatch.h 
  listmatch.cpp 
  plan.h 
  plan.cpp 
  submit_request.h 
  submit_request.cpp 
  cancel_request.h 
  cancel_request.cpp 
  match_request.h 
  match_request.cpp 
  events.h 
  events.cpp 
  dispatcher_utils.h 
  dispatcher_utils.cpp 
  wm_real.h 
  wm_real.cpp 
  signal_handling.h 
  signal_handling.cpp 
  match_utils.h 
  match_utils.cpp 
  recovery.h 
  recovery.cpp 
  lb_utils.h 
  lb_utils.cpp 
  dynamic_library.h 
  dynamic_library.cpp 
  submission_utils.h 
  submission_utils.cpp 
  replanner.cpp)

install(TARGETS glite-wms-workload_manager
        DESTINATION ${PREFIX}/${BINDIR})

target_link_libraries(glite-wms-workload_manager
  ${WMSUTILS_CLASSADS_LIBRARIES}
  ${WMSUTILS_JOBMAN_EXCEPTION_LIBRARIES}
  ${JDL_API_CPP_LIBRARIES}
  ${BOOST_REGEX_LIBRARIES}
  ${BOOST_FILESYSTEM_LIBRARIES}
  ${BOOST_THREAD_LIBRARIES}
  expat
  ${WMS_COMMON_CONF_LIBRARIES}
  ${WMS_COMMON_UTIL_LIBRARIES}
  ${JOBID_LIBRARIES}
  ${BOOST_REGEX_LIBRARIES}
  ${BOOST_FILESYSTEM_LIBRARIES}
  ${BOOST_DATE_TIME_LIBRARIES}
  )




#install(FILES 
#  ${CMAKE_CURRENT_SOURCE_DIR}/manipulation.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/boost_fs_add.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/streamdescriptor.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/LineParserExceptions.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/quota.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/triple.h
#  DESTINATION
#  ${PREFIX}/include/glite/wms/common/utilities)




#if(DEFINED INSTALL_BUILD_DEV)
#  install(TARGETS glite_wms_util
#    LIBRARY DESTINATION ${INSTALL_BUILD_DEV}/${LIBDIR})
#  
#  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/manipulation.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/boost_fs_add.h
#    DESTINATION
#    ${INSTALL_BUILD_DEV}/include/glite/wms/common/utilities)
#  
#endif(DEFINED INSTALL_BUILD_DEV)




#if(DEFINED INSTALL_BUILD)
#  install(TARGETS glite_wms_util
#    LIBRARY DESTINATION ${INSTALL_BUILD}/${LIBDIR})
#  install(TARGETS glite-wms-quota-adjust
#    DESTINATION ${INSTALL_BUILD}/${SBINDIR})
#  install(TARGETS glite-wms-eval_ad_expr
#    DESTINATION ${INSTALL_BUILD}/${LIBEXEC})
#endif(DEFINED INSTALL_BUILD)
