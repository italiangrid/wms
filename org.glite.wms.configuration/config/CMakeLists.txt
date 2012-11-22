configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-check-daemons.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-check-daemons.sh
  @ONLY)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-parse-configuration.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-parse-configuration.sh
  @ONLY)

#install(FILES 
#  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-check-daemons.sh
#  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-services-certs.sh
#  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-parse-configuration.sh
#  DESTINATION
#  ${PREFIX}/${LIBEXEC}
#  )

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

add_custom_target(ZIPMAN ALL DEPENDS 
  ${PREFIX}/../opt/glite/share/man/man1/glite-WMS.1.gz
  )

add_custom_command(OUTPUT 
  ${PREFIX}/${LIBEXEC}/glite-wms-check-daemons.sh
  ${PREFIX}/${LIBEXEC}/glite-wms-services-certs.sh
  ${PREFIX}/${LIBEXEC}/glite-wms-parse-configuration.sh
  ${PREFIX}/../etc/wms.conf.template
  COMMAND
  mkdir -p ${PREFIX}/${LIBEXEC}
  COMMAND
  cp ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-check-daemons.sh ${PREFIX}/${LIBEXEC}/
  COMMAND
  cp ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-services-certs.sh ${PREFIX}/${LIBEXEC}/
  COMMAND
  cp ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-parse-configuration.sh ${PREFIX}/${LIBEXEC}/
  COMMAND 
  chmod 755 ${PREFIX}/${LIBEXEC}/glite-wms-check-daemons.sh 
  COMMAND
  chmod 755 ${PREFIX}/${LIBEXEC}/glite-wms-services-certs.sh 
  COMMAND
  chmod 755 ${PREFIX}/${LIBEXEC}/glite-wms-parse-configuration.sh
  COMMAND 
  mkdir -p ${PREFIX}/../etc/
  COMMAND
  cp -f glite_wms.conf ${PREFIX}/../etc/wms.conf.template
)

add_custom_target(MAKEEXESCRIPTS ALL DEPENDS 
  ${PREFIX}/${LIBEXEC}/glite-wms-check-daemons.sh
  ${PREFIX}/${LIBEXEC}/glite-wms-services-certs.sh
  ${PREFIX}/${LIBEXEC}/glite-wms-parse-configuration.sh
  )

