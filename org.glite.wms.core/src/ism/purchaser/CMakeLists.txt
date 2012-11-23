
include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PROJECT_SOURCE_DIR}/src)

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
  ${WMS_COMMON_LOGGER_LIBRARIES}
  ${BOOST_REGEX_LIBRARIES}
  ${BOOST_THREAD_LIBRARIES}
  ${BOOST_DATE_TIME_LIBRARIES}
  ${BOOST_FILESYSTEM_LIBRARIES}
  ${CLASSAD_LIBRARIES}
  ldap
  )


install(TARGETS glite_wms_ism_ii_purchaser
  LIBRARY 
  DESTINATION 
  ${PREFIX}/${LIBDIR}
  )
