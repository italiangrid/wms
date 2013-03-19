configure_file(${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-lm.in
               ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-lm
               @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-jc.in
               ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-jc
               @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-clean-lm-recycle.sh.in
               ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-clean-lm-recycle.sh
               @ONLY)

install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-lm
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-jc
  DESTINATION 
  ${SYSCONFIGDIR}/init.d/
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-clean-lm-recycle.sh
  DESTINATION
  ${PREFIX}/stage/usr/${LIBEXEC}/
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )
