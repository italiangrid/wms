#!/bin/csh
#######################################################
##              GUI startup script                   ##
##  Starts the Job Submitter component from which    ##
##      all other components can be accessed         ##
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
setenv RGMAJ_PATH ${RGMAJ_INSTALL_PATH}

setenv EXTRA_CLASSES ${GLITE_WMS_LOCATION}/share/java/glite-wms-jdlj.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${GLITE_WMS_LOCATION}/share/java/glite-wms-ui-api-java.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${GLITE_WMS_LOCATION}/share/java/glite-wms-ui-gui-java.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${GLITE_WMS_LOCATION}/share/java/glite-security-util-java.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${GLITE_WMS_LOCATION}/share/glite-security-trustmanager.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${GLITE_WMS_LOCATION}/share/java/glite-rgma-stubs-servlet-java.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${GLITE_WMS_LOCATION}/share/java/glite-rgma-api-java.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${JGLOBUS_PATH}/cog-jglobus.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${LOG4J_PATH}/log4j-1.2.8.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${CLASSADJ_PATH}/classad.jar
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:/opt/glite/externals/share/java/bcprov-jdk14-122.jar




# Job Submitter specific
setenv EXTRA_CLASSES ${EXTRA_CLASSES}:${RGMAJ_PATH}/share/java/info.jar

exec "${JAVA_PATH}/bin/java" -DRGMA_PROPS=$RGMA_PROPS -cp ".:${EXTRA_CLASSES}" org.glite.wmsui.guij.JobSubmitter &
