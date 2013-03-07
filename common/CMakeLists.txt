
message("*\n*\n*\n* Configuring build for wms.common component\n*\n*\n*")

set(PROJECT_SOURCE_DIR "${MAIN_PROJECT_SOURCE_DIR}/common/")

add_definitions(${CLASSAD_CFLAGS})
  
add_subdirectory(src)
add_subdirectory(interface)
