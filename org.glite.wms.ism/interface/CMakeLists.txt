install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/ism
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/glite/wms
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/ism/purchaser
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/glite/wms/ism/
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)
