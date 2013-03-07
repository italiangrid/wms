configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wms-jss-logmonitor.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/wms-jss-logmonitor.pc
               @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wms-jss-controller-wrapper.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/wms-jss-controller-wrapper.pc
               @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wms-jss-controller.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/wms-jss-controller.pc
               @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wms-jss-controller-adapter.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/wms-jss-controller-adapter.pc
               @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wms-jss-common.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/wms-jss-common.pc
               @ONLY)


install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
  DESTINATION ${PREFIX}/${LIBDIR}/pkgconfig
  FILES_MATCHING PATTERN "*.pc"
  PATTERN "CMakeFiles" EXCLUDE)
