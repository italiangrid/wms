configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-check-daemons.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-check-daemons.sh
  @ONLY)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-parse-configuration.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-parse-configuration.sh
  @ONLY)

install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/functions
  DESTINATION ${PREFIX}/stage/opt/glite/yaim
  PATTERN "CMakeFiles" EXCLUDE
)

install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/node-info.d
  DESTINATION ${PREFIX}/stage/opt/glite/yaim
  PATTERN "CMakeFiles" EXCLUDE
)

install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/defaults
  DESTINATION ${PREFIX}/stage/opt/glite/yaim
  PATTERN "CMakeFiles" EXCLUDE
)


install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/services
  DESTINATION ${PREFIX}/stage/opt/glite/yaim
  PATTERN "CMakeFiles" EXCLUDE
)

add_custom_command(OUTPUT 
  man/glite-WMS.1.gz
  COMMAND mkdir -p ${PREFIX}/stage/opt/glite/share/man/man1
  COMMAND gzip -c man/glite-WMS.1 > man/glite-WMS.1.gz
)

add_custom_target(ZIPMAN ALL DEPENDS 
  man/glite-WMS.1.gz
  )
  
install(FILES
  man/glite-WMS.1.gz
  DESTINATION
  ${PREFIX}/stage/opt/glite/share/man/man1/
  )
  
add_custom_command(OUTPUT 
  wms.conf.template
  COMMAND
  cp -f glite_wms.conf wms.conf.template
)

install(FILES
  glite-wms-check-daemons.sh
  glite-wms-services-certs.sh
  glite-wms-parse-configuration.sh
  DESTINATION
  ${PREFIX}/stage/usr/${LIBEXEC}/
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )
  
install(FILES
  wms.conf.template 
  DESTINATION
  ${SYSCONFIGDIR}/glite-wms/
  )

add_custom_target(MAKEEXESCRIPTS ALL DEPENDS 
  wms.conf.template
  )

