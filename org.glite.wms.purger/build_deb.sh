#!/bin/bash

INITIALPWD=${PWD}

PKGVERSION=3.5.0
PKGAGE=1
PKGNAME=libglite-wms-purger

PRJNAME=org.glite.wms.purger

set -e

if [ "x$1" == "x-s" ]; then
    mkdir -p SOURCES
    tar --exclude .git --exclude debian --exclude build* -zcf ${PKGNAME}_${PKGVERSION}.orig.tar.gz org.glite.wms/${PRJNAME} 
fi

mkdir -p BINARIES org.glite.wms/${PRJNAME}/debian/source

###########################################################################
#
# Control file
#
###########################################################################
cat << EOF > org.glite.wms/${PRJNAME}/debian/control
Source:  ${PKGNAME}
Section:  libs
Priority:  optional
Maintainer:  WMS Support <wms-support@cnaf.infn.it>
Build-Depends: debhelper (>= 8.0.0~), cmake, libglite-wms-common, libglite-wms-common-dev, emi-pkgconfig-compat,
 libglite-wms-utils-classad-dev , libglite-wms-utils-exception-dev, libclassad0-dev, libboost-system1.42-dev, libboost1.42-dev, libglite-lb-client-dev, libglite-lb-client11, libglite-lb-common-dev, libglite-lb-common13,libglobus-gss-assist-dev,libglobus-gss-assist3,libglobus-common-dev,libglobus-common0,libglobus-openssl-dev,libglobus-openssl,libglite-jobid-api-cpp-dev,libglite-jobid2,libc-ares-dev, libc-ares2,liblog4cpp5-dev,liblog4cpp5
Standards-Version:  3.5.0
Homepage: http://glite.cern.ch/

Package:  ${PKGNAME}
Architecture: any
Depends: debhelper (>= 8.0.0~), cmake, libglite-wms-common, libglite-wms-common-dev, emi-pkgconfig-compat,
 libglite-wms-utils-classad-dev , libglite-wms-utils-exception-dev, libclassad0-dev, libboost-system1.42-dev, libboost1.42-dev, libglite-lb-client-dev, libglite-lb-client11, libglite-lb-common-dev, libglite-lb-common13,libglobus-gss-assist-dev,libglobus-gss-assist3,libglobus-common-dev,libglobus-common0,libglobus-openssl-dev,libglobus-openssl,libglite-jobid-api-cpp-dev,libglite-jobid2,libc-ares-dev, libc-ares2,liblog4cpp5-dev,liblog4cpp5,\${shlibs:Depends}, \${misc:Depends}
Description:  WMS purger

Package:  ${PKGNAME}-dev
Architecture: any
Depends: debhelper (>= 8.0.0~), cmake, libglite-wms-common, libglite-wms-common-dev, emi-pkgconfig-compat,
 libglite-wms-utils-classad-dev , libglite-wms-utils-exception-dev, libclassad0-dev, libboost-system1.42-dev, libboost1.42-dev, libglite-lb-client-dev, libglite-lb-client11, libglite-lb-common-dev, libglite-lb-common13,libglobus-gss-assist-dev,libglobus-gss-assist3,libglobus-common-dev,libglobus-common0,libglobus-openssl-dev,libglobus-openssl,libglite-jobid-api-cpp-dev,libglite-jobid2,libc-ares-dev, libc-ares2,liblog4cpp5-dev,liblog4cpp5,\${shlibs:Depends}, \${misc:Depends}
Description:  WMS purger development files

EOF

###########################################################################
#
# Copyright file
#
###########################################################################
cat << EOF > org.glite.wms/${PRJNAME}/debian/copyright
Format-Specification: http://svn.debian.org/wsvn/dep/web/deps/dep5.mdwn?op=file&rev=135
Name: ${PKGNAME}
Maintainer: WMS Support <wms-support@cnaf.infn.it>
Source: http://glite.cern.ch/


Files: *
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.
License: Apache
 On Debian systems, the full text of the Apache License version 2 can be found
 in the file /usr/share/common-licenses/Apache-2.0.

EOF

###########################################################################
#
# Installation files
#
###########################################################################
cat << EOF > org.glite.wms/${PRJNAME}/debian/${PKGNAME}.install
usr/share/doc/glite-wms-purger-3.5.0/LICENSE
usr/lib/libglite_wms_purger*.so.*
usr/sbin/glite-wms-purgeStorage
usr/sbin/glite-wms-purgeStorage.sh
usr/sbin/glite-wms-create-proxy.sh
EOF

cat << EOF > org.glite.wms/${PRJNAME}/debian/${PKGNAME}-dev.install
usr/lib/libglite_wms_purger*.so
usr/lib/pkgconfig/wms-purger.pc
usr/include/glite/wms/purger/purger.h
EOF

###########################################################################
#
# Rule file
#
###########################################################################
cat << EOF > org.glite.wms/${PRJNAME}/debian/rules
#!/usr/bin/make -f
export DH_COMPAT=7
#export DH_VERBOSE=1

INSTALLDIR=\$(CURDIR)/debian/tmp

build-stamp:
	touch build-stamp
	
build:build-stamp
	cmake -DPREFIX:string=\$(INSTALLDIR)/usr -DPVER:string=${PKGVERSION} \$(CURDIR)
	make

clean:
	dh_testdir
	dh_testroot
	rm -rf build-stamp
	rm -rf configure-stamp
	rm -rf \$(INSTALLDIR)
	dh_clean
	find -iname '*cmake*' -not -name CMakeLists.txt -exec rm -rf {} \+

install: build
	dh_testdir
	dh_testroot
	dh_prep
	dh_installdirs
	make install
	cmake -DPREFIX:string=${INITIALPWD}/STAGE/usr -DPVER:string=${PKGVERSION} \$(CURDIR)
	make install


binary-indep: build install

binary-arch: build install
	dh_testdir
	dh_testroot
	dh_installdocs
	dh_installman
	dh_installchangelogs
	dh_install
	dh_link
	dh_strip
	dh_compress
	dh_fixperms
	dh_installdeb
	dh_makeshlibs

        # temporary disable untill proxyrenewal will be disable
        # otherwise this command will fail on the self built proxyrenewal library (built by us by hand)
	#dh_shlibdeps
	dh_gencontrol
	dh_md5sums
	dh_builddeb --destdir=${PWD}/BINARIES

binary: binary-indep binary-arch
.PHONY: build clean binary-indep binary-arch binary install
EOF

###########################################################################
#
# Package format
#
###########################################################################
cat << EOF > org.glite.wms/${PRJNAME}/debian/source/format
3.0 (quilt)
EOF

###########################################################################
#
# Changelog
#
###########################################################################
cat << EOF > org.glite.wms/${PRJNAME}/debian/changelog
${PKGNAME} (${PKGVERSION}-${PKGAGE}) stable; urgency=low

  * New major release

 -- WMS Support <wms-support@cnaf.infn.it> Fri, 31 Aug 2012 00:00:00 +0000

EOF


if [ "x$1" == "x-s" ]; then
    dpkg-source -i.* -b org.glite.wms/${PRJNAME}
    mv ${PKGNAME}_${PKGVERSION}* SOURCES
fi
cd org.glite.wms/${PRJNAME}
fakeroot make -f debian/rules binary
rm -rf build debian build-stamp
cd -