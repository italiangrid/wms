configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wms-purger.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/wms-purger.pc
               @ONLY)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
        DESTINATION ${PREFIX}/${LIBDIR}/pkgconfig
        FILES_MATCHING PATTERN "*.pc"
        PATTERN "CMakeFiles" EXCLUDE)
