#!/bin/sh
#######################################################
##           Job Monitor startup script              ##
#######################################################

GLITE_LOCATION=${GLITE_LOCATION:-/opt/glite}
GLITE_WMS_LOCATION=${GLITE_WMS_LOCATION:-${GLITE_LOCATION}}

. ${GLITE_WMS_LOCATION}/etc/glite-wmsui-vars.sh
. ${GLITE_WMS_LOCATION}/etc/profile.d/glite-wmsui.sh
. ${GLITE_WMS_LOCATION}/etc/profile.d/glite-wmsui-gui-env.sh

JAVA_PATH=${JAVA_INSTALL_PATH}
JGLOBUS_PATH=${JGLOBUS_INSTALL_PATH}
LOG4J_PATH=${LOG4J_INSTALL_PATH}
CLASSADJ_PATH=${CLASSADJ_INSTALL_PATH}
RGMAJ_PATH=${RGMAJ_INSTALL_PATH}

EXTRA_CLASSES="$GLITE_WMS_LOCATION/share/java/glite-wms-jdlj.jar"
EXTRA_CLASSES="$EXTRA_CLASSES:$GLITE_WMS_LOCATION/share/java/glite-wms-ui-api-java.jar"
EXTRA_CLASSES="$EXTRA_CLASSES:$GLITE_WMS_LOCATION/share/java/glite-wms-ui-gui-java.jar"
EXTRA_CLASSES="$EXTRA_CLASSES:$GLITE_WMS_LOCATION/share/java/glite-security-util-java.jar"
EXTRA_CLASSES="$EXTRA_CLASSES:$GLITE_WMS_LOCATION/share/java/glite-security-trustmanager.jar"
EXTRA_CLASSES="$EXTRA_CLASSES:$GLITE_WMS_LOCATION/share/java/glite-rgma-stubs-servlet-java.jar"
EXTRA_CLASSES="$EXTRA_CLASSES:$GLITE_WMS_LOCATION/share/java/glite-rgma-api-java.jar"
EXTRA_CLASSES="$EXTRA_CLASSES:$JGLOBUS_PATH/cog-jglobus.jar"
EXTRA_CLASSES="$EXTRA_CLASSES:$LOG4J_PATH/log4j-1.2.8.jar"
EXTRA_CLASSES="$EXTRA_CLASSES:$CLASSADJ_PATH/classad.jar"
EXTRA_CLASSES="$EXTRA_CLASSES:/opt/glite/externals/share/java/bcprov-jdk14-122.jar"


# Job Monitor specific
EXTRA_CLASSES="$EXTRA_CLASSES:$GLITE_WMS_LOCATION/share/java/info.jar"
exec "$JAVA_PATH/bin/java" -DRGMA_PROPS=$RGMA_PROPS -cp ".:$EXTRA_CLASSES" org.glite.wmsui.guij.JobMonitor &
