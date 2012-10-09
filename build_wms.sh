#!/bin/sh

autotools_build()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   TMP_DIR=$5
   cd $COMPONENT
   if [ false ]; then # TODO --force option or skip the configuration is makefiles have been created already
      echo -e "\n*** Makefile present, skipping configuration\n"
      cd build
   else
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
      if [ $? -ne 0 ]; then
         echo ERROR
         exit
      fi
   fi
   make
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   make doxygen-doc >/dev/null 2>&1
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

cmake_build()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   TMP_DIR=$5
   cd $COMPONENT
   echo TODO
}

ant_build()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   TMP_DIR=$5
   cd $COMPONENT
   ant clean
   mkdir -p build src/autogen rpmbuild/SOURCES rpmbuild/SPECS rpmbuild/SRPMS rpmbuild/BUILD rpmbuild/RPMS 2>/dev/null
   tar --exclude rpmbuild --exclude build --exclude bin --exclude tools -zcf \
      ./rpmbuild/SOURCES/${PACKAGE_NAME}-${VERSION}-${AGE}.${PLATFORM}.tar.gz .
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   printf "dist.location=${TMP_DIR} stage.location=${STAGE_DIR} module.version=${VERSION}" > .configuration.properties
   ant
   ant install
   ${BUILD_DIR}/org.glite.wms/emi-jobman-rpm-tool --pack --pkgname ${PACKAGE_NAME} \
	--version ${VERSION}-${AGE} --distro $PLATFORM --localdir ${TMP_DIR} \
	--specfile ${BUILD_DIR}/org.glite.wms/$COMPONENT/project/${PACKAGE_NAME}.spec
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   cd $BUILD_DIR/org.glite.wms
}


python_build()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   TMP_DIR=$5
   cd $COMPONENT
   python setup.py clean --all
   mkdir -p build src/autogen rpmbuild/SOURCES rpmbuild/SPECS rpmbuild/SRPMS rpmbuild/BUILD rpmbuild/RPMS 2>/dev/null
   tar --exclude rpmbuild --exclude build --exclude bin --exclude tools -zcf \
      ./rpmbuild/SOURCES/${PACKAGE_NAME}-${VERSION}-${AGE}.${PLATFORM}.tar.gz .
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   printf "[global] pkgversion=${version} " > setup.cfg
   python setup.py install -O1 --prefix ${TMP_DIR}/usr --install-data ${TMP_DIR}
   ${BUILD_DIR}/org.glite.wms/emi-jobman-rpm-tool --pack --pkgname ${PACKAGE_NAME} \
	--version ${VERSION}-${AGE} --distro $PLATFORM --localdir ${TMP_DIR} \
	--specfile ${BUILD_DIR}/org.glite.wms/$COMPONENT/project/${PACKAGE_NAME}.spec
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   cd $BUILD_DIR/org.glite.wms
}

rpmbuild()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   TMP_DIR=$5
   cd $COMPONENT
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
   echo "wms <build-dir-name> <emi-release> <os> <want_external_deps> <want_vcs_checkout> <want_cleanup> (missing: <tag>)"
   exit
fi

#wget --no-check-certificate https://github.com/MarcoCecchi/org.glite.wms.git/build_wms.sh -o build_wms.sh
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

echo -e "\n*** starting build for WMS service ***\n"

COMPONENT[0]=org.glite.wms.common
COMPONENT[1]=org.glite.wms.ism
COMPONENT[2]=org.glite.wms.helper
COMPONENT[3]=org.glite.wms.purger
COMPONENT[4]=org.glite.wms.jobsubmission
COMPONENT[5]=org.glite.wms.ice
COMPONENT[6]=org.glite.wms.manager
COMPONENT[7]=org.glite.wms.wmproxy
COMPONENT[8]=org.glite.wms.nagios
COMPONENT[9]=org.glite.wms.mp
### WMS UI
COMPONENT[10]=org.glite.wms.brokerinfo-access
COMPONENT[11]=org.glite.wms.wmproxy-api-cpp
COMPONENT[12]=org.glite.wms.wmproxy-api-java
COMPONENT[13]=org.glite.wms.wmproxy-api-python
COMPONENT[14]=org.glite.wms-ui.api-python
COMPONENT[15]=org.glite.wms-ui.commands

BUILD_TYPE[0]=autotools
BUILD_TYPE[1]=autotools
BUILD_TYPE[2]=autotools
BUILD_TYPE[3]=autotools
BUILD_TYPE[4]=autotools
BUILD_TYPE[5]=autotools
BUILD_TYPE[6]=autotools
BUILD_TYPE[7]=autotools
BUILD_TYPE[8]=rpmbuild
BUILD_TYPE[9]=rpmbuild
BUILD_TYPE[10]=autotools
BUILD_TYPE[11]=autotools
BUILD_TYPE[12]=ant
BUILD_TYPE[13]=python
BUILD_TYPE[14]=autotools # NOT python
BUILD_TYPE[15]=autotools

PACKAGE_NAME[0]=glite-wms-common
PACKAGE_NAME[1]=glite-wms-ism
PACKAGE_NAME[2]=glite-wms-helper
PACKAGE_NAME[3]=glite-wms-purger
PACKAGE_NAME[4]=glite-wms-jobsubmission
PACKAGE_NAME[5]=glite-wms-ice
PACKAGE_NAME[6]=glite-wms-manager
PACKAGE_NAME[7]=glite-wms-wmproxy
PACKAGE_NAME[8]=glite-wms-nagios
PACKAGE_NAME[9]=glite-wms-mp
PACKAGE_NAME[10]=glite-wms-brokerinfo-access
PACKAGE_NAME[11]=glite-wms-wmproxy-api-cpp
PACKAGE_NAME[12]=glite-wms-wmproxy-api-java
PACKAGE_NAME[13]=glite-wms-wmproxy-api-python
PACKAGE_NAME[14]=glite-wms-ui-api-python
PACKAGE_NAME[15]=glite-wms-ui-commands

VERSION[0]=3.5.0
VERSION[1]=3.5.0
VERSION[2]=3.5.0
VERSION[3]=3.5.0 # purger
VERSION[4]=3.5.0 # jobsubmission
VERSION[5]=3.5.0
VERSION[6]=3.5.0
VERSION[7]=3.5.0
VERSION[8]=3.5.0
VERSION[9]=3.5.0
VERSION[10]=3.5.0
VERSION[11]=3.5.0
VERSION[12]=3.5.0
VERSION[13]=3.5.0
VERSION[14]=3.5.0
VERSION[15]=3.5.0

AGE[0]=0
AGE[1]=0
AGE[2]=0
AGE[3]=0 # purger
AGE[4]=0 # jobsubmission
AGE[5]=0
AGE[6]=0
AGE[7]=0
AGE[8]=0
AGE[9]=0
AGE[10]=0
AGE[11]=0
AGE[12]=0
AGE[13]=0
AGE[14]=0
AGE[15]=0

for i in `seq 0 9`; do
   echo -e "\n*** building component ${COMPONENT[$i]} ***\n"
   
   if [ $6 -ne 0 ]; then
      cd ${COMPONENT[$i]}
      echo "\n*** cleaning up ${COMPONENT[$i]} ***\n"
      make -C build clean 2>/dev/null
      rm -rf rpmbuild tgz RPMS 2>/dev/null
      cd ..
      exit
   fi

   case ${BUILD_TYPE[$i]} in
     "autotools" )
         autotools_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} "$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}/tmp"
         ;;
     "cmake" )
         ant_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} "$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}/tmp"
         ;;
     "ant" )
         ant_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} "$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}/tmp"
         ;;
     "python" )
         python_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} "$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}/tmp"
         ;;
     "rpm" )
         rpmbuild ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} "$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}/tmp"
         ;;
     * )
         echo "Build type not found for ${COMPONENT[$i]}"
         exit
         ;;
   esac
done
