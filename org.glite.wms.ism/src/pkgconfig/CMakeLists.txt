configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wms-ism-ii-purchaser.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/wms-ism-ii-purchaser.pc
               @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wms-ism.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/wms-ism.pc
               @ONLY)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR}/pkgconfig
        FILES_MATCHING PATTERN "*.pc"
        PATTERN "CMakeFiles" EXCLUDE)

if(DEFINED INSTALL_BUILD_DEV)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
    DESTINATION ${INSTALL_BUILD_DEV}/${LIBDIR}/pkgconfig
    FILES_MATCHING PATTERN "*.pc"
    PATTERN "CMakeFiles" EXCLUDE)
endif(DEFINED INSTALL_BUILD_DEV)

