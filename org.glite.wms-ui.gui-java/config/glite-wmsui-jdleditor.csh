#!/bin/csh
#######################################################
##          JDL Editor startup script                ##
#######################################################

if ( ! $?GLITE_LOCATION ) then
  setenv GLITE_LOCATION "/opt/glite"
endif

if ( ! $?GLITE_WMS_LOCATION ) then
  setenv GLITE_WMS_LOCATION ${GLITE_LOCATION}
endif

source ${GLITE_WMS_LOCATION}/etc/glite-wmsui-vars.csh
source ${GLITE_WMS_LOCATION}/etc/profile.d/glite-wmsui.csh
source ${GLITE_WMS_LOCATION}/etc/profile.d/glite-wmsui-gui-env.csh

setenv JAVA_PATH ${JAVA_INSTALL_PATH}
setenv JGLOBUS_PATH ${JGLOBUS_INSTALL_PATH}
setenv LOG4J_PATH ${LOG4J_INSTALL_PATH}
setenv CLASSADJ_PATH ${CLASSADJ_INSTALL_PATH}

setenv EXTRA_CLASSES ${GLITE_WMS_LOCATION}/share/java/glite-wms-jdlj.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${GLITE_WMS_LOCATION}/share/java/glite-wms-ui-api-java.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${GLITE_WMS_LOCATION}/share/java/glite-wms-ui-gui-java.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${JGLOBUS_PATH}/cog-jglobus.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${LOG4J_PATH}/log4j-1.2.8.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${CLASSADJ_PATH}/classad.jar

exec "${JAVA_PATH}/bin/java" -cp ".:${EXTRA_CLASSES}" org.glite.wmsui.guij.JDLEditor &
