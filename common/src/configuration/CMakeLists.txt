add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/src/configuration/CommonConfiguration.h DEPENDS ${PROJECT_SOURCE_DIR}/src/configuration/CommonConfiguration.h.G COMMAND ${PROJECT_SOURCE_DIR}/src/scripts/generator.pl ${PROJECT_SOURCE_DIR}/src/configuration/Configuration.def -H ${PROJECT_SOURCE_DIR}/src/configuration/CommonConfiguration.h.G)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/src/configuration/cwrapper.cpp DEPENDS ${PROJECT_SOURCE_DIR}/src/configuration/cwrapper.cpp.G COMMAND ${PROJECT_SOURCE_DIR}/src/scripts/generator.pl ${PROJECT_SOURCE_DIR}/src/configuration/Configuration.def -c ${PROJECT_SOURCE_DIR}/src/configuration/cwrapper.cpp.G)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/src/configuration/cwrapper.h DEPENDS ${PROJECT_SOURCE_DIR}/src/configuration/cwrapper.h.G COMMAND ${PROJECT_SOURCE_DIR}/src/scripts/generator.pl ${PROJECT_SOURCE_DIR}/src/configuration/Configuration.def -H ${PROJECT_SOURCE_DIR}/src/configuration/cwrapper.h.G)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/src/configuration/ICEConfiguration.h DEPENDS ${PROJECT_SOURCE_DIR}/src/configuration/ICEConfiguration.h.G COMMAND ${PROJECT_SOURCE_DIR}/src/scripts/generator.pl ${PROJECT_SOURCE_DIR}/src/configuration/Configuration.def -H ${PROJECT_SOURCE_DIR}/src/configuration/ICEConfiguration.h.G)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/src/configuration/JCConfiguration.h DEPENDS ${PROJECT_SOURCE_DIR}/src/configuration/JCConfiguration.h.G COMMAND ${PROJECT_SOURCE_DIR}/src/scripts/generator.pl ${PROJECT_SOURCE_DIR}/src/configuration/Configuration.def -H ${PROJECT_SOURCE_DIR}/src/configuration/JCConfiguration.h.G)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/src/configuration/LMConfiguration.h DEPENDS ${PROJECT_SOURCE_DIR}/src/configuration/LMConfiguration.h.G COMMAND ${PROJECT_SOURCE_DIR}/src/scripts/generator.pl ${PROJECT_SOURCE_DIR}/src/configuration/Configuration.def -H ${PROJECT_SOURCE_DIR}/src/configuration/LMConfiguration.h.G)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/src/configuration/NSConfiguration.h DEPENDS ${PROJECT_SOURCE_DIR}/src/configuration/NSConfiguration.h.G COMMAND ${PROJECT_SOURCE_DIR}/src/scripts/generator.pl ${PROJECT_SOURCE_DIR}/src/configuration/Configuration.def -H ${PROJECT_SOURCE_DIR}/src/configuration/NSConfiguration.h.G)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/src/configuration/WMCConfiguration.h DEPENDS ${PROJECT_SOURCE_DIR}/src/configuration/WMCConfiguration.h.G COMMAND ${PROJECT_SOURCE_DIR}/src/scripts/generator.pl ${PROJECT_SOURCE_DIR}/src/configuration/Configuration.def -H ${PROJECT_SOURCE_DIR}/src/configuration/WMCConfiguration.h.G)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/src/configuration/WMConfiguration.h DEPENDS ${PROJECT_SOURCE_DIR}/src/configuration/WMConfiguration.h.G COMMAND ${PROJECT_SOURCE_DIR}/src/scripts/generator.pl ${PROJECT_SOURCE_DIR}/src/configuration/Configuration.def -H ${PROJECT_SOURCE_DIR}/src/configuration/WMConfiguration.h.G)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/src/configuration/WMPConfiguration.h DEPENDS ${PROJECT_SOURCE_DIR}/src/configuration/WMPConfiguration.h.G COMMAND ${PROJECT_SOURCE_DIR}/src/scripts/generator.pl ${PROJECT_SOURCE_DIR}/src/configuration/Configuration.def -H ${PROJECT_SOURCE_DIR}/src/configuration/WMPConfiguration.h.G)

add_custom_target(GENCONF ALL DEPENDS ${PROJECT_SOURCE_DIR}/src/configuration/CommonConfiguration.h 
			      	      ${PROJECT_SOURCE_DIR}/src/configuration/cwrapper.cpp 
				      ${PROJECT_SOURCE_DIR}/src/configuration/cwrapper.h 
				      ${PROJECT_SOURCE_DIR}/src/configuration/ICEConfiguration.h 
				      ${PROJECT_SOURCE_DIR}/src/configuration/JCConfiguration.h 
				      ${PROJECT_SOURCE_DIR}/src/configuration/LMConfiguration.h 
				      ${PROJECT_SOURCE_DIR}/src/configuration/NSConfiguration.h 
				      ${PROJECT_SOURCE_DIR}/src/configuration/WMCConfiguration.h 
				      ${PROJECT_SOURCE_DIR}/src/configuration/WMConfiguration.h 
				      ${PROJECT_SOURCE_DIR}/src/configuration/WMPConfiguration.h)
				      
#SET_SOURCE_FILES_PROPERTIES(${PROJECT_SOURCE_DIR}/src/configuration/CommonConfiguration.h PROPERTIES GENERATED 1)

link_directories(${PREFIX}/stage/usr/${LIBDIR})



include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
                    ${PREFIX}/stage/usr/include
                    ${WMSUTILS_CLASSADS_ALT_INCLUDE_DIRS}
                    ${WMSUTILS_CLASSADS_INCLUDE_DIRS}
		    ${PROJECT_SOURCE_DIR}/interface/glite/wms/common
                    ${PROJECT_SOURCE_DIR}/src)


add_custom_target(CONF DEPENDS 
  CommonConfiguration.cpp
  confbase.cpp
  Configuration.cpp
  cwrapper.cpp
  exceptions.cpp
  ICEConfiguration.cpp
  initializer.cpp
  JCConfiguration.cpp
  LMConfiguration.cpp
  ModuleType.cpp
  NSConfiguration.cpp
  WMCConfiguration.cpp
  WMConfiguration.cpp
  WMPConfiguration.cpp
)

add_dependencies( CONF GENCONF )
#ADD_DEPENDENCIES(CommonConfiguration.cpp generate_CommonConfiguration.h)


add_library(glite_wms_conf SHARED 
  CommonConfiguration.cpp
  confbase.cpp
  Configuration.cpp
  cwrapper.cpp
  exceptions.cpp
  ICEConfiguration.cpp
  initializer.cpp
  JCConfiguration.cpp
  LMConfiguration.cpp
  ModuleType.cpp
  NSConfiguration.cpp
  WMCConfiguration.cpp
  WMConfiguration.cpp
  WMPConfiguration.cpp)

add_dependencies( glite_wms_conf GENCONF )

set_target_properties(glite_wms_conf PROPERTIES VERSION 0.0.0 SOVERSION 0)



install(TARGETS glite_wms_conf 
        LIBRARY DESTINATION ${PREFIX}/stage/usr/${LIBDIR})



install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION
	${PREFIX}/stage/usr/include/glite/wms/common/configuration
        FILES_MATCHING PATTERN "*.h"
        PATTERN "CMakeFiles" EXCLUDE)
