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

