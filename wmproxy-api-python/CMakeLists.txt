
message("*\n*\n*\n* Configuring build for wms.wmproxy-api-python component\n*\n*\n*")

set(PROJECT_SOURCE_DIR "${MAIN_PROJECT_SOURCE_DIR}/wmproxy-api-python/")
 
message("Installing wmproxy-api-python...")
EXEC_PROGRAM(/usr/bin/python ARGS setup.py install -O1
--prefix ${PREFIX} --install-data ${PREFIX}
 OUTPUT_VARIABLE PYOUT)
 
