configure_file(${CMAKE_CURRENT_SOURCE_DIR}/glite-wmsutils-jobman-exception.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/glite-wmsutils-jobman-exception.pc
               @ONLY)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR}/pkgconfig
        FILES_MATCHING PATTERN "*.pc"
        PATTERN "CMakeFiles" EXCLUDE)
