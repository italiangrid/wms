
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PROJECT_SOURCE_DIR}/src)

add_custom_target(UTILITIES DEPENDS boost_fs_add.cpp     FileListIterator.cpp
glite-wms-eval_ad_expr.cpp  globus_ftp_get.cpp    input_reader.cpp
lb_server.cpp             manipulation.cpp
dynamic_library.cpp  FileListLock.cpp      glite-wms-fl2jd.cpp
globus_ftp_mkdir.cpp  jobdir.cpp         lb_utils.cpp              mixed.cpp
filecontainer.cpp    FileLocker.cpp        glite_wms_quota_adjust.cpp
globus_ftp_put.cpp    jobdir_reader.cpp  LineParser.cpp            quota.cpp
FileList.cpp         fstreamlock.cpp       globus_ftp_exists.cpp
#ii_attr_utils.cpp     
lb_proxy.cpp       LineParserExceptions.cpp
wm_commands.cpp)

add_library(glite_wms_util SHARED LineParser.cpp LineParserExceptions.cpp
mixed.cpp boost_fs_add.cpp wm_commands.cpp jobdir.cpp input_reader.cpp
jobdir_reader.cpp manipulation.cpp)

#add_library(glite_wms_iiattrutil SHARED  ii_attr_utils.cpp)

set_target_properties(glite_wms_util PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wms_util 	${BOOST_LIBS}
					${CLASSAD_LIBS}
        				${GLITE_WMSUTILS_CLASSADS_LIBS}
				        ${GLITE_JOBID_LIBS}
					${CLASSAD_LIBRARIES})

#set_target_properties(glite_wms_iiattrutil PROPERTIES VERSION 0.0.0 SOVERSION 0)

add_executable(glite-wms-quota-adjust glite_wms_quota_adjust.cpp)
install(TARGETS glite-wms-quota-adjust
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${BINDIR})
target_link_libraries(glite-wms-quota-adjust 
                                                glite_wms_conf 
                                                glite_wms_util 
                                                glite_wmsutils_classads
                                                glite_jobid
                                                boost_regex 
                                                boost_filesystem
                                                boost_date_time
                                                )


install(TARGETS glite_wms_util #glite_wms_iiattrutil
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR})
