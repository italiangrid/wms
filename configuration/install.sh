#!/bin/bash

PREFIX=$1
VERSION=$2

mkdir -p ${PREFIX}/stage/opt/glite/yaim
mkdir -p ${PREFIX}/stage/opt/glite/yaim/defaults
mkdir -p ${PREFIX}/stage/opt/glite/yaim/services
mkdir -p ${PREFIX}/stage/opt/glite/yaim/examples/siteinfo/services/
mkdir -p ${PREFIX}/stage/opt/glite/yaim/functions
mkdir -p ${PREFIX}/stage/opt/glite/share/man/man1
mkdir -p ${PREFIX}/stage/opt/glite/yaim/node-info.d
mkdir -p ${PREFIX}/usr/libexec
mkdir -p ${PREFIX}/etc/glite-wms
mkdir -p ${PREFIX}/usr/share/doc/glite-wms-configuration-${VERSION}

cp config/glite-wms-check-daemons.sh.in ${PREFIX}/usr/libexec/glite-wms-check-daemons.sh
cp config/glite-wms-parse-configuration.sh.in ${PREFIX}/usr/libexec/glite-wms-parse-configuration.sh
cp config/glite-wms-services-certs.sh ${PREFIX}/usr/libexec/glite-wms-services-certs.sh

cp config/defaults/* ${PREFIX}/stage/opt/glite/yaim/defaults/
gzip config/man/glite-WMS.1
cp config/man/glite-WMS.1.gz ${PREFIX}/stage/opt/glite/share/man/man1
cp config/functions/* ${PREFIX}/stage/opt/glite/yaim/functions/
cp config/services/glite-wms ${PREFIX}/stage/opt/glite/yaim/services/
cp config/node-info.d/* ${PREFIX}/stage/opt/glite/yaim/node-info.d/

chmod a+x ${PREFIX}/usr/libexec/glite-wms-check-daemons.sh
chmod a+x ${PREFIX}/usr/libexec/glite-wms-parse-configuration.sh
chmod a+x ${PREFIX}/usr/libexec/glite-wms-services-certs.sh

cp config/glite_wms.conf ${PREFIX}/etc/glite-wms/wms.conf.template

cp LICENSE ${PREFIX}/usr/share/doc/glite-wms-configuration-${VERSION}
chmod a-x ${PREFIX}/usr/share/doc/glite-wms-configuration-${VERSION}/LICENSE
