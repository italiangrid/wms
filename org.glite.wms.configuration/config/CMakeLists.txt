configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-check-daemons.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-check-daemons.sh
  @ONLY)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-parse-configuration.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-parse-configuration.sh
  @ONLY)

install(FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-check-daemons.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-services-certs.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-parse-configuration.sh
  DESTINATION
  ${PREFIX}/${LIBEXEC}
  )

install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/functions
  DESTINATION ${PREFIX}/../opt/glite/yaim
  PATTERN "CMakeFiles" EXCLUDE
)

install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/node-info.d
  DESTINATION ${PREFIX}/../opt/glite/yaim
  PATTERN "CMakeFiles" EXCLUDE
)

install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/defaults
  DESTINATION ${PREFIX}/../opt/glite/yaim
  PATTERN "CMakeFiles" EXCLUDE
)

#install(FILES
#  ${CMAKE_CURRENT_SOURCE_DIR}/man/glite-WMS.1
#  DESTINATION ${PREFIX}/../opt/glite/share/man/man1
#)

install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/services
  DESTINATION ${PREFIX}/../opt/glite/yaim
  PATTERN "CMakeFiles" EXCLUDE
)

add_custom_command(OUTPUT 
  ${PREFIX}/../opt/glite/share/man/man1/glite-WMS.1.gz
  COMMAND mkdir -p ${PREFIX}/../opt/glite/share/man/man1
  COMMAND gzip -c ${CMAKE_CURRENT_SOURCE_DIR}/man/glite-WMS.1 > ${PREFIX}/../opt/glite/share/man/man1/glite-WMS.1.gz
)

add_custom_target(GENPYC ALL DEPENDS 
  ${PREFIX}/../opt/glite/share/man/man1/glite-WMS.1.gz
  )
