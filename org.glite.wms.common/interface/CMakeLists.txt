install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/common/logger
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/glite/wms/common
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/common/utilities
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/glite/wms/common
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)


#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/common/ldif2classad
#        DESTINATION
#${CMAKE_INSTALL_PREFIX}/include/glite/wms/common/ldif2classad
#        FILES_MATCHING PATTERN "*.h"
#        PATTERN "CMakeFiles" EXCLUDE)


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/common/configuration
        DESTINATION
${CMAKE_INSTALL_PREFIX}/include/glite/wms/common
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)



