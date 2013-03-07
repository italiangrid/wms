message("*\n*\n*\n* Configuring build for wms.ice component\n*\n*\n*")

set(PROJECT_SOURCE_DIR "${MAIN_PROJECT_SOURCE_DIR}/ice/")

if(EXISTS "/etc/redhat-release")
  set(ONRH "1")
endif(EXISTS "/etc/redhat-release")

if(DEFINED ONRH)
EXEC_PROGRAM(cat
  ARGS  /etc/redhat-release | /usr/bin/awk '{print $(NF-1)}' | /usr/bin/awk -F"." '{print $1}'
  OUTPUT_VARIABLE RH_VER)
endif(DEFINED ONRH)

add_definitions( -DICE_VERSIONID="${ICE_VERSIONID}" -DHAVE_GLITE_JOBID -DGLITE_WMS_ICE_ENABLE_STATS -UHAVE_CONFIG_H  ${CLASSAD_CFLAGS} )

add_subdirectory(src)
add_subdirectory(config)
