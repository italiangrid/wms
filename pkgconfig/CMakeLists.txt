configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wms-server.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/wms-server.pc
               @ONLY)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
        DESTINATION ${PREFIX}/usr/${LIBDIR}/pkgconfig
        FILES_MATCHING PATTERN "*.pc"
        PATTERN "CMakeFiles" EXCLUDE)
