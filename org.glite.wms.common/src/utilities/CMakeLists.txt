
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PREFIX}/include
		    ${WMSUTILS_CLASSADS_ALT_INCLUDE_DIRS}
		    ${WMSUTILS_CLASSADS_INCLUDE_DIRS}
		    ${PROJECT_SOURCE_DIR}/src)

link_directories(${PREFIX}/${LIBDIR})

add_custom_target(UTILITIES 
  DEPENDS 
  boost_fs_add.cpp     
  FileListIterator.cpp
  glite-wms-eval_ad_expr.cpp  
  globus_ftp_get.cpp    
  input_reader.cpp
  lb_server.cpp             
  manipulation.cpp
  dynamic_library.cpp  
  FileListLock.cpp      
  glite-wms-fl2jd.cpp
  globus_ftp_mkdir.cpp  
  jobdir.cpp         
  lb_utils.cpp              
  mixed.cpp
  filecontainer.cpp    
  FileLocker.cpp        
  glite_wms_quota_adjust.cpp
  globus_ftp_put.cpp    
  jobdir_reader.cpp  
  LineParser.cpp            
  quota.cpp
  FileList.cpp         
  fstreamlock.cpp       
  globus_ftp_exists.cpp
  lb_proxy.cpp       
  LineParserExceptions.cpp
  wm_commands.cpp)

add_library(glite_wms_util 
  SHARED 
  LineParser.cpp 
  LineParserExceptions.cpp
  mixed.cpp 
  boost_fs_add.cpp 
  wm_commands.cpp 
  jobdir.cpp 
  input_reader.cpp
  jobdir_reader.cpp 
  manipulation.cpp)

add_library(glite_wms_quota
  SHARED 
  quota.cpp)

set_target_properties(glite_wms_util PROPERTIES VERSION 0.0.0 SOVERSION 0)
set_target_properties(glite_wms_quota PROPERTIES VERSION 0.0.0 SOVERSION 0)


add_executable(glite-wms-quota-adjust glite_wms_quota_adjust.cpp)
install(TARGETS glite-wms-quota-adjust
        DESTINATION ${PREFIX}/${SBINDIR})

target_link_libraries(glite-wms-quota-adjust 
  ${WMSUTILS_CLASSADS_ALT_LIBRARIES}
  ${WMSUTILS_CLASSADS_LIBRARIES}
  glite_wms_conf 
  glite_wms_util 
  glite_jobid
  boost_regex 
  boost_filesystem
  boost_date_time
  )

add_executable(glite-wms-eval_ad_expr glite-wms-eval_ad_expr.cpp)
install(TARGETS glite-wms-eval_ad_expr
        DESTINATION ${PREFIX}/${LIBEXEC})


target_link_libraries(glite-wms-eval_ad_expr
  ${WMSUTILS_CLASSADS_ALT_LIBRARIES}
  ${WMSUTILS_CLASSADS_LIBRARIES}
  glite_wms_conf 
  glite_wms_util 
  glite_jobid
  boost_regex 
  boost_filesystem
  boost_date_time
  )

install(TARGETS glite_wms_util glite_wms_quota
        LIBRARY DESTINATION ${PREFIX}/${LIBDIR})


install(FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/manipulation.h
  ${CMAKE_CURRENT_SOURCE_DIR}/boost_fs_add.h
  ${CMAKE_CURRENT_SOURCE_DIR}/streamdescriptor.h
  ${CMAKE_CURRENT_SOURCE_DIR}/LineParserExceptions.h
  ${CMAKE_CURRENT_SOURCE_DIR}/quota.h
  ${CMAKE_CURRENT_SOURCE_DIR}/triple.h
  DESTINATION
  ${PREFIX}/include/glite/wms/common/utilities)




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
