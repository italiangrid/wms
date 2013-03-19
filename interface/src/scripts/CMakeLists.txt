configure_file(${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-wmproxy-purge-proxycache_keys.in
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-wmproxy-purge-proxycache_keys
  @ONLY
  )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-wmproxy-purge-proxycache.in
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-wmproxy-purge-proxycache
  @ONLY
  )



install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-wmproxy-purge-proxycache_keys
  DESTINATION ${PREFIX}/usr/${BINDIR}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )

install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/glite-wms-wmproxy-purge-proxycache
  DESTINATION ${PREFIX}/usr/${BINDIR}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )


install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/glite_wms_wmproxy_load_monitor
  DESTINATION ${PREFIX}/usr/${SBINDIR}/
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )
