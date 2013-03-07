add_custom_command(OUTPUT 
  ${PREFIX}/share/man/man1/glite-wms-workload_manager.1.gz
  COMMAND mkdir -p ${PREFIX}/share/man/man1
  COMMAND gzip -c ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-workload_manager.1 >
${PREFIX}/share/man/man1/glite-wms-workload_manager.1.gz
  COMMAND gzip -c ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-stats.1 >
${PREFIX}/share/man/man1/glite-wms-stats.1.gz
  COMMAND gzip -c
${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-query-job-state-transitions.1 >
${PREFIX}/share/man/man1/glite-wms-query-job-state-transitions.1.gz
)

find_file(DB_MANPAGES_STYLESHEET "docbook.xsl"
          PATHS "/usr/share/xml/docbook/stylesheet/docbook-xsl/manpages"
                "/usr/share/sgml/docbook/xsl-stylesheets/manpages")

if( NOT DB_MANPAGES_STYLESHEET )
    message(FATAL_ERROR "Cannot find stylesheet for man pages")
endif( NOT DB_MANPAGES_STYLESHEET )

add_custom_command(OUTPUT
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-workload_manager.1
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-stats.1
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-query-job-state-transitions.1
  COMMAND
  xsltproc ${DB_MANPAGES_STYLESHEET}
${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-workload_manager.xml
  COMMAND
  xsltproc ${DB_MANPAGES_STYLESHEET}
${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-stats.xml
  COMMAND
  xsltproc ${DB_MANPAGES_STYLESHEET}
${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-query-job-state-transitions.xml
)

add_custom_target(ZIPMAN2 ALL DEPENDS 
  ${PREFIX}/share/man/man1/glite-wms-workload_manager.1.gz
  ${PREFIX}/share/man/man1/glite-wms-stats.1.gz
  ${PREFIX}/share/man/man1/glite-wms-query-job-state-transitions.1.gz
    )

add_custom_target(XMLPROC ALL DEPENDS 
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-workload_manager.1
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-stats.1
  ${CMAKE_CURRENT_SOURCE_DIR}/glite-wms-query-job-state-transitions.1
    )
