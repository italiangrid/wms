
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${MAIN_PROJECT_SOURCE_DIR}
		    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
		    ${MAIN_PROJECT_SOURCE_DIR}/purger/src
		    ${PREFIX}/include
		    ${WMSUTILS_CLASSADS_ALT_INCLUDE_DIRS}
		    ${WMSUTILS_CLASSADS_INCLUDE_DIRS}
		    ${WMS_COMMON_UTIL_INCLUDE_DIRS}
		    ${PURGER_INCLUDE_DIRS}
		    ${PROJECT_SOURCE_DIR}/src)

link_directories(${PREFIX}/${LIBDIR})

add_custom_target(COMMON 
  DEPENDS 
  fstreamlock.cpp
  id_container.cpp
  ram_container.cpp
  file_container.cpp
  pointer_id.cpp
  filelist.cpp
  filelist_iterator.cpp
  filelist_lock.cpp
  file_locker.cpp
  EventLogger.cpp
  JobFilePurger.cpp
  ProxyUnregistrar.cpp
  constants.cpp
  files.cpp
  LockFile.cpp
  SignalChecker.cpp
  handler.c
  )

add_library(glite_wms_jss_common
  SHARED 
  fstreamlock.cpp
  id_container.cpp
  ram_container.cpp
  file_container.cpp
  pointer_id.cpp
  filelist.cpp
  filelist_iterator.cpp
  filelist_lock.cpp
  file_locker.cpp
  EventLogger.cpp
  JobFilePurger.cpp
  ProxyUnregistrar.cpp
  constants.cpp
  files.cpp
  LockFile.cpp
  SignalChecker.cpp
  handler.c
  )

set_target_properties(glite_wms_jss_common PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_jss_common
  LIBRARY DESTINATION ${PREFIX}/${LIBDIR})
