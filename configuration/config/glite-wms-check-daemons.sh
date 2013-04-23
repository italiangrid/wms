#!/bin/sh

SCRIPT_LOCATION=${WMS_LOCATION_ETC}/init.d
WMS_PREFIX=glite-wms
LB_PREFIX=glite-lb
PROXY_RENEWAL=glite-proxy-renewald
GLITE_LB_LOCATION_VAR=${GLITE_LB_LOCATION_VAR:-'/var/glite'}
LL_PIDFILE="${GLITE_LB_LOCATION_VAR}/glite-lb-logd.pid"
BKS_PIDFILE="${GLITE_LB_LOCATION_VAR}/glite-lb-bkserverd.pid"
LM_PIDFILE=/var/run/glite-wms-log_monitor.pid # /var/run/ is hard-coded in the corresponding startup script
JC_PIDFILE=/var/run/glite-wms-job_controller.pid # same as above
ICE_PIDFILE="${WMS_LOCATION_VAR}/run/glite-wms-ice-safe.pid"
WM_PIDFILE="${WMS_LOCATION_VAR}/run/glite-wms-workload_manager.pid"

if [ -f ${SCRIPT_LOCATION}/${LB_PREFIX}-locallogger ]; then
  ${SCRIPT_LOCATION}/${LB_PREFIX}-locallogger status >/dev/null
  if [ $? -ne 0 ]; then
    if [ -s "$LL_PIDFILE" ]; then
      ${SCRIPT_LOCATION}/${LB_PREFIX}-locallogger restart
    fi
  fi
fi

if [ -f ${SCRIPT_LOCATION}/${LB_PREFIX}-bkserverd ]; then
  ${SCRIPT_LOCATION}/${LB_PREFIX}-bkserverd status >/dev/null
  if [ $? -ne 0 ]; then
    if [ -s "$BKS_PIDFILE" ]; then
      ${SCRIPT_LOCATION}/${LB_PREFIX}-bkserverd restart
    fi
  fi
fi

if [ -f ${SCRIPT_LOCATION}/${PROXY_} ]; then
  ${SCRIPT_LOCATION}/${PROXY_RENEWAL} status >/dev/null
  if [ $? -ne 0 ]; then
      ${SCRIPT_LOCATION}/${PROXY_RENEWAL} restart
  fi
fi

if [ -f ${SCRIPT_LOCATION}/${WMS_PREFIX}-lm ]; then
  ${SCRIPT_LOCATION}/${WMS_PREFIX}-lm status >/dev/null
  if [ $? -ne 0 ]; then
    if [ -s "$LM_PIDFILE" ]; then
      ${SCRIPT_LOCATION}/${WMS_PREFIX}-lm restart
    fi
  fi
fi

if [ -f ${SCRIPT_LOCATION}/${WMS_PREFIX}-jc ]; then
  ${SCRIPT_LOCATION}/${WMS_PREFIX}-jc status >/dev/null
  if [ $? -ne 0 ]; then
    if [ -s "$JC_PIDFILE" ]; then
      ${SCRIPT_LOCATION}/${WMS_PREFIX}-jc restart
    fi
  fi
fi

if [ -f ${SCRIPT_LOCATION}/${WMS_PREFIX}-ice ]; then
 ${SCRIPT_LOCATION}/${WMS_PREFIX}-ice status >/dev/null
  if [ $? -ne 0 ]; then
    if [ -s "$ICE_PIDFILE" ]; then
      ${SCRIPT_LOCATION}/${WMS_PREFIX}-ice restart
    fi
  fi
fi


#if [ -f ${SCRIPT_LOCATION}/${WMS_PREFIX}-ice ]; then
# ${SCRIPT_LOCATION}/${WMS_PREFIX}-ice status >/dev/null
#if [ $? -eq 1 ]; then
# ${SCRIPT_LOCATION}/${WMS_PREFIX}-ice restart
#fi
#fi


if [ -f ${SCRIPT_LOCATION}/${WMS_PREFIX}-wm ]; then
  ${SCRIPT_LOCATION}/${WMS_PREFIX}-wm status >/dev/null
  if [ $? -ne 0 ]; then
    if [ -s "$WM_PIDFILE" ]; then
      ${SCRIPT_LOCATION}/${WMS_PREFIX}-wm restart
    fi
  fi
fi
