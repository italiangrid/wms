message("*\n*\n*\n* Configuring build for wms.purger component\n*\n*\n*")

set(PROJECT_SOURCE_DIR "${MAIN_PROJECT_SOURCE_DIR}/purger/")

if(EXISTS "/etc/debian_version")
  set(ONDEB "1")
endif(EXISTS "/etc/debian_version")

if(EXISTS "/etc/redhat-release")
  set(ONRH "1")
endif(EXISTS "/etc/redhat-release")

if(DEFINED ONDEB)
EXEC_PROGRAM(cat
  ARGS /etc/debian_version | /usr/bin/awk -F"." '{print $1}'
  OUTPUT_VARIABLE DEB_VER)
endif(DEFINED ONDEB)

if(DEFINED ONRH)
EXEC_PROGRAM(cat
  ARGS  /etc/redhat-release | /usr/bin/awk '{print $(NF-1)}' | /usr/bin/awk -F"." '{print $1}'
  OUTPUT_VARIABLE RH_VER)
endif(DEFINED ONRH)

add_definitions( ${CLASSAD_CFLAGS})

add_subdirectory(src)
add_subdirectory(config)
