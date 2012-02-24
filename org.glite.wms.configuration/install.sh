#!/bin/bash

PREFIX=$1
VERSION=$2

mkdir -p ${PREFIX}/usr/libexec
mkdir -p ${PREFIX}/etc/glite-wms
mkdir -p ${PREFIX}/usr/share/doc/glite-wms-configuration-${VERSION}

cp config/glite-wms-check-daemons.sh.in ${PREFIX}/usr/libexec/glite-wms-check-daemons.sh
cp config/glite-wms-parse-configuration.sh.in ${PREFIX}/usr/libexec/glite-wms-parse-configuration.sh
cp config/glite-wms-services-certs.sh ${PREFIX}/usr/libexec/glite-wms-services-certs.sh

chmod a+x ${PREFIX}/usr/libexec/glite-wms-check-daemons.sh
chmod a+x ${PREFIX}/usr/libexec/glite-wms-parse-configuration.sh
chmod a+x ${PREFIX}/usr/libexec/glite-wms-services-certs.sh

cp config/glite_wms.conf ${PREFIX}/etc/glite-wms/wms.conf.template

cp LICENSE ${PREFIX}/usr/share/doc/glite-wms-configuration-${VERSION}
chmod a-x ${PREFIX}/usr/share/doc/glite-wms-configuration-${VERSION}/LICENSE

