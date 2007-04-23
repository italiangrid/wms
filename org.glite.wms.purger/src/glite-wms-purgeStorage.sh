#!/bin/bash

create_proxy()
{
   proxy=`"${GLOBUS_LOCATION}/bin/grid-proxy-init" -q \
     -cert $GLITE_HOST_CERT \
     -key  $GLITE_HOST_KEY \
     -hours 24 \
     -out  "$1.$$" 2>&1`
   
   [ $? -eq 0 ] || echo $proxy " Fail " >> $2

   move=`chmod 400 "$1.$$" &&
     chown "${GLITE_WMS_USER}" "$1.$$" &&
     mv "$1.$$" "$1" 2>&1`

   [ $? -eq 0 ] || echo $move " Fail " >> $2
}

check_process()
{
  ps=`ps ax | grep -v grep | grep "glite-wms-purgeStorage" > /dev/null`
  [ $? -eq 0 ] || echo "glite-wms-purgeStorage already running..." >> $2 && exit 1
}

log="/dev/null"
while getopts "l:t:c:t:p:a:b:f:e:q" arg
do 
  case "$arg" in 
  l) log="$OPTARG";
  esac
done  

create_proxy  "${GLITE_WMS_TMP}/purger.proxy" $log
check_process $log
glite-wms-purgeStorage "$@"
