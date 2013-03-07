message("*\n*\n*\n* Configuring build for wms.core component\n*\n*\n*")

set(PROJECT_SOURCE_DIR "${MAIN_PROJECT_SOURCE_DIR}/core/")

if(EXISTS "/etc/debian_version")
  set(ONDEB "1")
endif(EXISTS "/etc/debian_version")

if(EXISTS "/etc/redhat-release")
  set(ONRH "1")
endif(EXISTS "/etc/redhat-release")

add_definitions(${CLASSAD_CFLAGS} -DLDAP_DEPRECATED)

if(DEFINED ONDEB)
  add_definitions( -DGSOAP_279_TRICK )
endif(DEFINED ONDEB)

add_subdirectory(src)
add_subdirectory(interface)
add_subdirectory(doc)
add_subdirectory(config)
