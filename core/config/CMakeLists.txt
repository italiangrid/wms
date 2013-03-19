configure_file(${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-wm.in
               ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-wm
               @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/template.sh
               ${CMAKE_CURRENT_BINARY_DIR}/jobwrapper.template.sh
               @ONLY)

install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-wm
  DESTINATION ${SYSCONFIGDIR}/rc.d/init.d/
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
WORLD_READ WORLD_EXECUTE
  )

install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/jobwrapper.template.sh
  DESTINATION ${PREFIX}/usr/share/glite-wms
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
WORLD_READ WORLD_EXECUTE
  )
