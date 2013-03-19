
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PROJECT_SOURCE_DIR}/src
		    ${MAIN_PROJECT_SOURCE_DIR}
		    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
		    /usr/include)

add_custom_target(PURCHASER 
  DEPENDS 
  common.cpp
  ldap-utils.cpp
  ldap-dn-utils.cpp
  schema_utils.cpp
  ism-ii-purchaser.cpp
  ldap-utils-g2.cpp
  ldap-dn-utils-g2.cpp
)

add_library(glite_wms_ism_ii_purchaser
  SHARED 
  common.cpp
  ldap-utils.cpp
  ldap-dn-utils.cpp
  schema_utils.cpp
  ism-ii-purchaser.cpp
  ldap-utils-g2.cpp
  ldap-dn-utils-g2.cpp 
  )

set_target_properties(glite_wms_ism_ii_purchaser PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wms_ism_ii_purchaser
  ${WMS_COMMON_LOGGER_LDFLAGS}
#  ${WMS_COMMON_LOGGER_LIBRARIES}
  ${BOOST_REGEX_LDFLAGS}
  ${BOOST_THREAD_LDFLAGS}
  ${BOOST_DATE_TIME_LDFLAGS}
  ${BOOST_FILESYSTEM_LDFLAGS}
  ${CLASSAD_LDFLAGS}
  ldap
  )


install(TARGETS glite_wms_ism_ii_purchaser
  LIBRARY 
  DESTINATION 
  ${PREFIX}/usr/${LIBDIR}
  )
