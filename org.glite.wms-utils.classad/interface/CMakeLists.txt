install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wmsutils/classads
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/glite/wmsutils
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)
