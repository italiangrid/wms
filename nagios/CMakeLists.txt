
message("*\n*\n*\n* Configuring build for wms.nagios component\n*\n*\n*")

set(PROJECT_SOURCE_DIR "${MAIN_PROJECT_SOURCE_DIR}/nagios/")

EXEC_PROGRAM(/bin/mkdir ARGS -p ${PREFIX}/usr)

EXEC_PROGRAM(/bin/echo ARGS \"[global]\" > nagios/setup.cfg 2>/dev/null OUTPUT_VARIABLE FIRST)

EXEC_PROGRAM(/bin/echo ARGS \"pkgversion=${PVER}\" >> nagios/setup.cfg 2>/dev/null OUTPUT_VARIABLE SECOND)

EXEC_PROGRAM(cd ARGS nagios; ${PYTHON_EXECUTABLE} setup.py install -O1 --prefix ${PREFIX}/usr --install-data ${PREFIX}; cd - OUTPUT_VARIABLE THIRD)

add_custom_target(nagios)
	
add_custom_command(TARGET nagios
  COMMAND 
  mkdir -p ${PREFIX}/usr
  COMMAND
  /bin/echo \"[global]\" > setup.cfg
  COMMAND
  /bin/echo \"pkgversion=${PVER}\" >> setup.cfg
  COMMAND
  /usr/bin/python setup.py install -O1 --prefix ${PREFIX}/usr --install-data
${PREFIX}/usr
  )
