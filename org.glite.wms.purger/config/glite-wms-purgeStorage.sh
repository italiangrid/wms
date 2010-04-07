# Copyright (c) Members of the EGEE Collaboration. 2004.
# See http://project.eu-egee.org/partners/ for details on the
# copyright holders.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
# either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#!/bin/bash
source /etc/init.d/functions

check_process()
{
  pid=`pidofproc glite-wms-purgeStorage`
  if [ $? -eq 0 ] ; then
    echo "glite-wms-purgeStorage already running on pid " $pid  >> $2
    exit 1
  fi
}

log="/dev/stdout"
while getopts "l:t:c:p:a:soh" arg
do
  case "$arg" in
  l)    log="$OPTARG";;
  [?])  ${GLITE_WMS_LOCATION}/sbin/glite-wms-purgeStorage -h > $log
        exit 1;;
  esac
done

proxyfile=`${GLITE_WMS_LOCATION}/bin/glite-wms-get-configuration Common.HostProxyFile`
if [ $? -eq 1 ] ; then
  proxyfile="${GLITE_WMS_LOCATION_VAR}/wms.proxy"
fi

openssl x509 -in $proxyfile -checkend `expr 3600 \* 6` > /dev/null
if [ $? -eq  1 ] ; then
   ${GLITE_WMS_LOCATION}/sbin/glite-wms-create-proxy.sh
fi

check_process glite-wms-purgeStorage $log

# if another instance had been running the check_process
# would have already exited
${GLITE_WMS_LOCATION}/sbin/glite-wms-purgeStorage "$@"
