
find_file(DB_MANPAGES_STYLESHEET "docbook.xsl"
          PATHS "/usr/share/xml/docbook/stylesheet/docbook-xsl/manpages"
                "/usr/share/sgml/docbook/xsl-stylesheets/manpages")
if( NOT DB_MANPAGES_STYLESHEET )
    message(FATAL_ERROR "Cannot find stylesheet for man pages")
endif( NOT DB_MANPAGES_STYLESHEET )


add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/doc/glite-wms-log_monitor.1 ${PROJECT_SOURCE_DIR}/doc/glite-wms-log_monitor.1.gz
  DEPENDS ${PROJECT_SOURCE_DIR}/doc/glite-wms-log_monitor.xml
  COMMAND xsltproc ${DB_MANPAGES_STYLESHEET} ${PROJECT_SOURCE_DIR}/doc/glite-wms-log_monitor.xml
  COMMAND gzip -c -f ${PROJECT_SOURCE_DIR}/doc/glite-wms-log_monitor.1 > ${PROJECT_SOURCE_DIR}/doc/glite-wms-log_monitor.1.gz
  )

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/doc/glite-wms-job_controller.1 ${PROJECT_SOURCE_DIR}/doc/glite-wms-job_controller.1.gz
  DEPENDS ${PROJECT_SOURCE_DIR}/doc/glite-wms-job_controller.xml
  COMMAND xsltproc ${DB_MANPAGES_STYLESHEET} ${PROJECT_SOURCE_DIR}/doc/glite-wms-job_controller.xml
  COMMAND gzip -c -f ${PROJECT_SOURCE_DIR}/doc/glite-wms-job_controller.1 > ${PROJECT_SOURCE_DIR}/doc/glite-wms-job_controller.1.gz
  )


add_custom_target(GENMANS ALL 
  DEPENDS
  glite-wms-job_controller.1
  glite-wms-log_monitor.1
  )

install(FILES 
  glite-wms-job_controller.1.gz
  glite-wms-log_monitor.1.gz
  DESTINATION
  ${PREFIX}/usr/share/man/man1
)
