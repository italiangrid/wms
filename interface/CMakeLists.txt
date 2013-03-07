
message("*\n*\n*\n* Configuring build for wms.interface component\n*\n*\n*")

set(PROJECT_SOURCE_DIR "${MAIN_PROJECT_SOURCE_DIR}/interface/")

if(EXISTS "/etc/debian_version")
  set(ONDEB "1")
  set(GSOAP_VERSION "2_7_9")
  add_definitions( -DGSOAP_VERSION_NUM=20709 )
  #add_definitions( -DGSOAP_279_TRICK )
endif(EXISTS "/etc/debian_version")

if(EXISTS "/etc/redhat-release")
  set(ONRH "1")
endif(EXISTS "/etc/redhat-release")

if(DEFINED ONRH)
EXEC_PROGRAM(cat
  ARGS  /etc/redhat-release | /usr/bin/awk '{print $(NF-1)}' | /usr/bin/awk -F"." '{print $1}'
  OUTPUT_VARIABLE RH_VER)
endif(DEFINED ONRH)

add_definitions( -DHAVE_STRINGSTREAM -pipe -Wall ${CLASSAD_CFLAGS} )

if(DEFINED ONRH)
  if("${RH_VER}" STREQUAL "5")
    add_definitions( -DGSOAP_VERSION_NUM=20713 )
    set(GSOAP_VERSION "2_7_13")
  endif("${RH_VER}" STREQUAL "5")

  if("${RH_VER}" STREQUAL "6")
    add_definitions( -DGSOAP_VERSION_NUM=20716 )
    set(GSOAP_VERSION "2_7_16")
  endif("${RH_VER}" STREQUAL "6")
endif(DEFINED ONRH)

add_definitions( -DWITH_FASTCGI ${FCGI_CPP_CFLAGS} -DWMP_SOFTWARE_VERSION=\"${PVER}\" )

add_subdirectory(src)
add_subdirectory(config)
