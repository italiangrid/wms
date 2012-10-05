#!/bin/sh

build()
{
   cd $2
   if [ $1 -eq 0 ]; then
     etics_like_build $2 $3 $4 $5 $6
   else # cmake
     cmake_build $2 $3 $4 $5 $6
   fi
   cd $PWD
}

cmake_build()
{
   # TODO
   cmake . "-DPVER=$3-$4" # $COMPONENT $PACKAGE_NAME $STAGE_DIR PREFIX still missing
}

etics_like_build()
{
   cd $1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   PREFIX=%5
   STAGE_DIR=$6
   ${BUILD_DIR}/org.glite.wms/emi-jobman-rpm-tool --clean
   ${BUILD_DIR}/org.glite.wms/emi-jobman-rpm-tool --init --pkgname ${PACKAGE_NAME} --version ${VERSION}-${AGE} --distro $PLATFORM
   ./install.sh ${PREFIX} ${VERSION}
   ${BUILD_DIR}/org.glite.wmsemi-jobman-rpm-tool --pack --pkgname ${PACKAGE_NAME} \
      --version ${VERSION}-${AGE} --distro sl6 --localdir ${PREFIX} \
      --specfile ./project/${PACKAGE_NAME}_noarch.spec
}

if [ -z $1 ]; then
   echo "wms <build-dir> <emi-release> <platform>"
   exit
fi

PWD=`pwd`
BUILD_DIR=$1
STAGE_DIR=$BUILD_DIR/stage
EMI_RELEASE=$2
PLATFORM=$3
PREFIX=/usr

sudo chkconfig --level 0123456 yum-autoupdate off
sudo /etc/init.d/yum-autoupdate stop

echo installing external dependencies
wget http://emisoft.web.cern.ch/emisoft/dist/EMI/$EMI_RELEASE/RPM-GPG-KEY-emi
sudo rpm --import RPM-GPG-KEY-emi
sudo yum -y install yum-priorities
wget "http://emisoft.web.cern.ch/emisoft/dist/EMI/$EMI_RELEASE/sl6/x86_64/base/emi-release-${EMI_RELEASE}.0.0-1.$PLATFORM.noarch.rpm"
sudo rpm -ivh "emi-release-${EMI_RELEASE}.0.0-1.$PLATFORM.noarch.rpm"
sudo rpm -e --nodeps c-ares-1.7.0-6.el6.x86_64
sudo yum -y install mod_fcgid mod_ssl gridsite-apache httpd-devel zlib-devel boost-devel c-ares-devel glite-px-proxyrenewal-devel voms-devel voms-clients argus-pep-api-c-devel lcmaps-without-gsi-devel lcmaps-devel classads-devel glite-build-common-cpp gsoap-devel libtar-devel cmake globus-ftp-client globus-ftp-client-devel log4cpp-devel log4cpp glite-jobid-api-c glite-jobid-api-c-devel glite-jobid-api-cpp-devel openldap-devel glite-wms-utils-exception glite-wms-utils-classad

#sudo rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
echo checking out the whole project
cd $HOME; git clone --progress -v https://github.com/MarcoCecchi/org.glite.wms.git
cd org.glite.wms

echo starting build

COMPONENT=org.glite.wms.configuration
PACKAGE_NAME=glite-wms-configuration
VERSION=3.5.0
AGE=0
ETICS_CMAKE=0
build $ETICS_CMAKE $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.common
PACKAGE_NAME=glite-wms-common
VERSION=3.5.0
AGE=0
ETICS_CMAKE=1 # uses cmake
build $ETICS_CMAKE $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.ism
PACKAGE_NAME=glite-wms-ism
VERSION=3.5.0
AGE=0
ETICS_CMAKE=0
build $ETICS_CMAKE $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.helper
PACKAGE_NAME=glite-wms-helper
VERSION=3.5.0
AGE=0
ETICS_CMAKE=0
build $ETICS_CMAKE $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.jobsubmission
PACKAGE_NAME=glite-wms-jobsubmission
VERSION=3.5.0
AGE=0
ETICS_CMAKE=0
build $ETICS_CMAKE $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.manager
PACKAGE_NAME=glite-wms-manager
VERSION=3.5.0
AGE=0
ETICS_CMAKE=0
build $ETICS_CMAKE $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.ice
PACKAGE_NAME=glite-wms-ice
VERSION=3.5.0
AGE=0
ETICS_CMAKE=0
build $ETICS_CMAKE $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.purger
PACKAGE_NAME=glite-wms-purger
VERSION=3.5.0
AGE=0
ETICS_CMAKE=0
build $ETICS_CMAKE $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.wmproxy
PACKAGE_NAME=glite-wms-wmproxy
VERSION=3.5.0
AGE=0
ETICS_CMAKE=0
build $ETICS_CMAKE $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

COMPONENT=org.glite.wms.nagios
PACKAGE_NAME=glite-wms-nagios
VERSION=3.5.0
AGE=0
ETICS_CMAKE=0
build $ETICS_CMAKE $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR

#COMPONENT=org.glite.wms.mp
#PACKAGE_NAME=glite-wms-mp
#VERSION=3.5.0
#AGE=0
#ETICS_CMAKE=0
#build $ETICS_CMAKE $COMPONENT $VERSION $AGE $PACKAGE_NAME $PREFIX $STAGE_DIR
