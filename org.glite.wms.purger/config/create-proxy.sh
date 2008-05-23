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

create_proxy `${GLITE_WMS_LOCATION}/bin/glite-wms-get-configuration Common.HostProxyFile` | /usr/bin/logger
