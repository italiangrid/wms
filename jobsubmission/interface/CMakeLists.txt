install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/jobsubmission
        DESTINATION ${PREFIX}/usr/include/glite/wms/
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)
