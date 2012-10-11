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




if(DEFINED INSTALL_BUILD_DEV)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION
    ${INSTALL_BUILD_DEV}/include/glite/wms/common/configuration
    FILES_MATCHING PATTERN "*.h"
    PATTERN "CMakeFiles" EXCLUDE)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/common/logger
    DESTINATION ${INSTALL_BUILD_DEV}/include/glite/wms/common
    FILES_MATCHING PATTERN "*.h"
    PATTERN "CMakeFiles" EXCLUDE)
  
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glite/wms/common/utilities
    DESTINATION ${INSTALL_BUILD_DEV}/include/glite/wms/common
    FILES_MATCHING PATTERN "*.h"
    PATTERN "CMakeFiles" EXCLUDE)
endif(DEFINED INSTALL_BUILD_DEV)

