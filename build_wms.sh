#!/bin/sh

autotools_build()
{
COMPONENT=$1
VERSION=$2
AGE=$3
PACKAGE_NAME=$4
TMP_DIR=$5
cd $COMPONENT
#if [ -r build/src/Makefile && *force* ]; then
#   echo -e "\n*** Makefile present, skipping configuration\n"
#   cd build
#else
   mkdir -p build src/autogen rpmbuild/SOURCES rpmbuild/SPECS rpmbuild/SRPMS rpmbuild/BUILD rpmbuild/RPMS 2>/dev/null
   aclocal -I ${M4_LOCATION} && 
	libtoolize --force && autoheader && automake --foreign --add-missing --copy && \
	autoconf
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   tar --exclude rpmbuild --exclude build --exclude bin --exclude tools -zcf \
      ./rpmbuild/SOURCES/${PACKAGE_NAME}-${VERSION}-${AGE}.${PLATFORM}.tar.gz .
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   cd build
   ../configure --prefix=${TMP_DIR}/usr --sysconfdir=${TMP_DIR}/etc --disable-static PVER=${VERSION}
#fi
if [ $? -ne 0 ]; then
   echo ERROR
   exit
fi
make
if [ $? -ne 0 ]; then
   echo ERROR
   exit
fi
make install
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$TMP_DIR/$LOCAL_PKGCFG_LIB
if [ $? -ne 0 ]; then
   echo ERROR
   exit
fi
cd .. # from build to component root
${BUILD_DIR}/org.glite.wms/emi-jobman-rpm-tool --pack --pkgname ${PACKAGE_NAME} \
	--version ${VERSION}-${AGE} --distro $PLATFORM --localdir ${TMP_DIR} \
	--specfile ${BUILD_DIR}/org.glite.wms/$COMPONENT/project/${PACKAGE_NAME}.spec
if [ $? -ne 0 ]; then
   echo ERROR
   exit
fi
cd $BUILD_DIR/org.glite.wms
}

create_rpm()
{
   echo TODO
}

cmake_build()
{
   echo TODO
}

get_external_deps()
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
   sudo yum -y install mod_fcgid mod_ssl gridsite-apache httpd-devel zlib-devel \
      boost-devel c-ares-devel glite-px-proxyrenewal-devel voms-devel voms-clients \
      argus-pep-api-c-devel lcmaps-without-gsi-devel lcmaps-devel classads-devel \
      glite-build-common-cpp gsoap-devel libtar-devel cmake globus-ftp-client \
      globus-ftp-client-devel log4cpp-devel log4cpp glite-jobid-api-c \
      glite-jobid-api-c-devel glite-jobid-api-cpp-devel openldap-devel \
      glite-wms-utils-exception glite-wms-utils-classad \
      glite-wms-utils-exception-devel glite-wms-utils-classad-devel \
      chrpath cppunit-devel glite-jdl-api-cpp-devel glite-lb-client-devel \
      glite-lbjp-common-gsoap-plugin-devel condor-emi glite-ce-cream-client-api-c glite-ce-cream-client-devel
}

if [ -z $6 ]; then
   echo "wms <build-dir-name> <emi-release> <os> <want_external_deps> <want_vcs_checkout> <want_cleanup>"
   exit
fi

#wget --no-check-certificate https://github.com/MarcoCecchi/org.glite.wms.git/build_wms.sh -o 
BUILD_DIR=`pwd`/$1
STAGE_DIR=$BUILD_DIR/org.glite.wms/stage
if [ -d "/usr/lib64/" ]; then
   LOCAL_PKGCFG_LIB=usr/lib64/pkgconfig/
else
   LOCAL_PKGCFG_LIB=usr/lib/pkgconfig/
fi
export PKG_CONFIG_PATH=$STAGE_DIR/$LOCAL_PKGCFG_LIB
EMI_RELEASE=$2
PLATFORM=$3
M4_LOCATION=/usr/share/emi/build/m4
ARCH=`uname -i`
if [ $4 -eq 1 ]; then
   get_external_deps
fi

echo -e "\n*** build dir: $BUILD_DIR, platform: $PLATFORM, architecture: $ARCH ***\n"

if [ $5 -ne 0 ]; then
   sudo rm -rf "$BUILD_DIR"
   mkdir -p "$BUILD_DIR"
   cd "$BUILD_DIR"
   echo -e "\n*** checking out the WMS project ***\n"
   git clone --progress -v git@github.com:MarcoCecchi/org.glite.wms.git
   cd org.glite.wms
   mkdir "$STAGE_DIR"
   if [ $? -ne 0 ]; then
      echo "ERROR"
      exit
   fi
else
   cd "$BUILD_DIR"/org.glite.wms
   echo -e "\n*** NOT checking out the WMS project ***\n"
fi
mkdir -p $STAGE_DIR/$LOCAL_PKGCFG_LIB
cp org.glite.wms/project/emi-condorg.pc $STAGE_DIR/$LOCAL_PKGCFG_LIB
export PKG_CONFIG_PATH=$STAGE_DIR/$LOCAL_PKGCFG_LIB

echo -e "\n*** starting build ***\n"

COMPONENT[0]=org.glite.wms.common
COMPONENT[1]=org.glite.wms.ism
COMPONENT[2]=org.glite.wms.helper
COMPONENT[3]=org.glite.wms.purger
COMPONENT[4]=org.glite.wms.jobsubmission
COMPONENT[5]=org.glite.wms.ice
COMPONENT[6]=org.glite.wms.manager
COMPONENT[7]=org.glite.wms.wmproxy

PACKAGE_NAME[0]=glite-wms-common
PACKAGE_NAME[1]=glite-wms-ism
PACKAGE_NAME[2]=glite-wms-helper
PACKAGE_NAME[3]=glite-wms-purger
PACKAGE_NAME[4]=glite-wms-jobsubmission
PACKAGE_NAME[5]=glite-wms-ice
PACKAGE_NAME[6]=glite-wms-manager
PACKAGE_NAME[7]=glite-wms-wmproxy

VERSION[0]=3.5.0
VERSION[1]=3.5.0
VERSION[2]=3.5.0
VERSION[3]=3.5.0 # purger
VERSION[4]=3.5.0 # jobsubmission
VERSION[5]=3.5.0
VERSION[6]=3.5.0
VERSION[7]=3.5.0

AGE[0]=0
AGE[1]=0
AGE[2]=0
AGE[3]=0 # purger
AGE[4]=0 # jobsubmission
AGE[5]=0
AGE[6]=0
AGE[7]=0

if [ $6 -ne 0 ]; then
   for i in `seq 0 7`; do
      cd ${COMPONENT[$i]}
      echo "\n*** cleaning up ${COMPONENT[$i]} ***\n"
      make -C build clean 2>/dev/null
      rm -rf rpmbuild tgz RPMS 2>/dev/null
      cd ..
   done
   exit
fi

for i in `seq 0 7`; do
   echo -e "\n*** building component ${COMPONENT[$i]} ***\n"
   autotools_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} "$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}/tmp"
done

COMPONENT[0]=org.glite.wms.configuration
COMPONENT[1]=org.glite.wms.nagios
COMPONENT[2]=org.glite.wms.mp

PACKAGE_NAME[0]=glite-wms-configuration
PACKAGE_NAME[1]=glite-wms-nagios
PACKAGE_NAME[2]=glite-wms-mp

VERSION[0]=3.5.0 # configuration
VERSION[1]=3.5.0 # nagios
VERSION[2]=3.5.0 # mp

AGE[0]=0 # configuration
AGE[1]=0 # nagios
AGE[2]=0 # mp

for i in `seq 0 2`; do
   echo -e "\n*** building component ${COMPONENT[$i]} ***\n"
   create_rpm ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} "$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}/tmp"
done
