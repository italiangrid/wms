install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/helper
        DESTINATION ${PREFIX}/usr/include/glite/wms
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/ism
        DESTINATION ${PREFIX}/usr/include/glite/wms
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)
