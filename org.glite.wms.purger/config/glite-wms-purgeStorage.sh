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
  ps ax | grep -v grep | grep -v $1.sh | grep $1 > /dev/null
  if [ $? -eq 0 ] ; then
    echo "glite-wms-purgeStorage already running..." >> $2
    exit 1
  fi
}

#create_proxy  "${GLITE_LOCATION_VAR}/purger.proxy" $log
#exit 1

log="/dev/stdout"
while getopts "l:t:c:p:a:bfeqh" arg
do
  case "$arg" in
  l)    log="$OPTARG";;
  [?])  ${GLITE_WMS_LOCATION}/sbin/glite-wms-purgeStorage -h > $log
        exit 1;;
  esac
done


create_proxy  "${GLITE_LOCATION_VAR}/wms.proxy" $log
check_process glite-wms-purgeStorage $log

# if another instance had been running the check_process
# would have already exited
${GLITE_WMS_LOCATION}/sbin/glite-wms-purgeStorage "$@"
