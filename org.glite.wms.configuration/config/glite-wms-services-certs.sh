/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://project.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

if [ -f /etc/sysconfig/glite ]; then
   . /etc/sysconfig/glite
fi

if [ -z "$GLITE_USER_CERT" ]; then
  export GLITE_HOST_CERT=/etc/grid-security/hostcert.pem
else
  export GLITE_HOST_CERT=$GLITE_USER_CERT
fi



if [ -z "$GLITE_USER_KEY" ]; then
  export GLITE_HOST_KEY=/etc/grid-security/hostkey.pem
else
  export GLITE_HOST_KEY=$GLITE_USER_KEY
fi

