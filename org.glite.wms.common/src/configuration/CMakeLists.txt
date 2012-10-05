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


include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
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
				    WMPConfiguration.cpp)

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

set_target_properties(glite_wms_conf PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wms_conf 	${BOOST_LIBS}
					${CLASSAD_LIBS}
					${GLITE_WMSUTILS_CLASSADS_LIBS}
					${CLASSAD_LIBRARIES})

#set_target_properties(glite_wms_iiattrutil PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_conf #glite_wms_iiattrutil
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR})
