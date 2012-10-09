install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/helper
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/glite/wms
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)
