#!/bin/sh

cmake_build()
{
   # TODO
   #cmake . "-DPVER=$3-$4" # $$PACKAGE_NAME $STAGE_DIR PREFIX still missing
}

autotools_build()
{
VERSION=$1
AGE=$2
PACKAGE_NAME=$3
PREFIX=$4
STAGE_DIR=$5
make -C ${BUILD_DIR} clean && ${BUILD_DIR}/org.glite.wms/emi-jobman-rpm-tool --clean
mkdir -p src/autogen && aclocal -I ${M4_LOCATION=} && 
	\libtoolize --force && autoheader && automake --foreign --add-missing --copy && \
	autoconf && \
	${STAGE_DIR}/usr/bin/emi-jobman-rpm-tool --init --pkgname ${PACKAGE_NAME} --version ${VERSION}-${AGE} --distro sl6
mkdir -p ${BUILD_DIR} && \
	cd ${BUILD_DIR} && \
	./configure --prefix=${PREFIX}/usr --disable-static PVER=${VERSION}
make -C ${BUILD_DIR}
make -C ${BUILD_DIR} install
${STAGE_DIR}/usr/bin/emi-jobman-rpm-tool --pack --pkgname ${PACKAGE_NAME} \
	--version ${VERSION}-${AGE} --distro sl6 --localdir ${PREFIX} \
	--specfile ./project/${PACKAGE_NAME}_${DISTRO}_${ARCH}.spec
}

create_rpm()
{
VERSION=$1
AGE=$2
PACKAGE_NAME=$3
PREFIX=$4
STAGE_DIR=$5
${BUILD_DIR}/org.glite.wms/emi-jobman-rpm-tool --clean
${BUILD_DIR}/org.glite.wms/emi-jobman-rpm-tool --init --pkgname ${PACKAGE_NAME} --version ${VERSION}-${AGE} --distro $PLATFORM
./install.sh ${PREFIX} ${VERSION}
${BUILD_DIR}/org.glite.wms/emi-jobman-rpm-tool --pack --pkgname ${PACKAGE_NAME} \
	--version ${VERSION}-${AGE} --distro sl6 --localdir ${PREFIX} \
	--specfile ./project/${PACKAGE_NAME}_noarch.spec
}

external_deps()
{
sudo chkconfig --level 0123456 yum-autoupdate off
sudo /etc/init.d/yum-autoupdate stop
echo installing external dependencies
wget http://emisoft.web.cern.ch/emisoft/dist/EMI/$EMI_RELEASE/RPM-GPG-KEY-emi
sudo rpm --import RPM-GPG-KEY-emi
sudo yum -y install yum-priorities
wget "http://emisoft.web.cern.ch/emisoft/dist/EMI/$EMI_RELEASE/sl6/x86_64/base/emi-release-${EMI_RELEASE}.0.0-1.$PLATFORM.noarch.rpm"
sudo rpm -ivh "emi-release-${EMI_RELEASE}.0.0-1.$PLATFORM.noarch.rpm"
sudo rpm -e --nodeps c-ares-1.7.0-6.el6.x86_64
sudo yum -y install mod_fcgid mod_ssl gridsite-apache httpd-devel zlib-devel boost-devel c-ares-devel glite-px-proxyrenewal-devel voms-devel voms-clients argus-pep-api-c-devel lcmaps-without-gsi-devel lcmaps-devel classads-devel glite-build-common-cpp gsoap-devel libtar-devel cmake globus-ftp-client globus-ftp-client-devel log4cpp-devel log4cpp glite-jobid-api-c glite-jobid-api-c-devel glite-jobid-api-cpp-devel openldap-devel glite-wms-utils-exception glite-wms-utils-classad glite-wms-utils-exception-devel glite-wms-utils-classad-devel chrpath cppunit-devel
}

if [ -z $1 ]; then
   echo "wms <build-dir-abs-path> <emi-release> <platform> <want_external_deps>"
   exit
fi

if [ $4 -eq 1 ]; then
   external_deps
fi
PWD=`pwd`
BUILD_DIR=$1
STAGE_DIR=$BUILD_DIR/stage
EMI_RELEASE=$2
PLATFORM=$3
PREFIX=$STAGE_DIR
M4_LOCATION=/usr/share/emi/build/m4

#set -x
sudo rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
echo checking out the whole project
git clone --progress -v https://github.com/MarcoCecchi/org.glite.wms.git
cd org.glite.wms

echo starting build

COMPONENT=org.glite.wms.configuration
PACKAGE_NAME=glite-wms-configuration
VERSION=3.5.0
AGE=0
create_rpm $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR
exit

COMPONENT=org.glite.wms.common
PACKAGE_NAME=glite-wms-common
VERSION=3.5.0
AGE=0
autotools_build $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.ism
PACKAGE_NAME=glite-wms-ism
VERSION=3.5.0
AGE=0
autotools_build $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.helper
PACKAGE_NAME=glite-wms-helper
VERSION=3.5.0
AGE=0
build $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.jobsubmission
PACKAGE_NAME=glite-wms-jobsubmission
VERSION=3.5.0
AGE=0
build $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.manager
PACKAGE_NAME=glite-wms-manager
VERSION=3.5.0
AGE=0
build $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.ice
PACKAGE_NAME=glite-wms-ice
VERSION=3.5.0
AGE=0
build $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.purger
PACKAGE_NAME=glite-wms-purger
VERSION=3.5.0
AGE=0
build $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.wmproxy
PACKAGE_NAME=glite-wms-wmproxy
VERSION=3.5.0
AGE=0
build $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.nagios
PACKAGE_NAME=glite-wms-nagios
VERSION=3.5.0
AGE=0
build $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

#COMPONENT=org.glite.wms.mp
#PACKAGE_NAME=glite-wms-mp
#VERSION=3.5.0
#AGE=0
#build $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR
