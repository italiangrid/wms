#!/bin/bash

autotools_build()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   TMP_DIR=$5

   cd $COMPONENT
   mkdir -p build src/autogen rpmbuild/SOURCES rpmbuild/SPECS rpmbuild/SRPMS rpmbuild/BUILD rpmbuild/RPMS 2>/dev/null
   cp -R $BUILD_DIR/org.glite.wms/org.glite.wms-ui/doxygen project/doxygen 2>/dev/null # needed by some UI component
   aclocal -I ${M4_LOCATION} && \
	libtoolize --force && autoheader && automake --foreign --add-missing --copy && \
	autoconf
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   if [ $COMPONENT = "org.glite.wms.wmproxy" ]; then # TODO hack required to integrate not os provided gsoap
      if [ $PLATFORM = "sl6" ]; then
         ln -sf "$BUILD_DIR/org.glite.wms/org.glite.wms.wmproxy/src/server/stdsoap2-2_7_16.cpp" \
            "$BUILD_DIR/org.glite.wms/org.glite.wms.wmproxy/src/server/stdsoap2.cpp"
      elif [ $PLATFORM = "sl5" ]; then 
         ln -sf "$BUILD_DIR/org.glite.wms/org.glite.wms.wmproxy/src/server/stdsoap2-2_7_13.cpp" \
            "$BUILD_DIR/org.glite.wms/org.glite.wms.wmproxy/src/server/stdsoap2.cpp"
      fi
   fi
   # create the source tarball before configure
   tar --exclude reports --exclude rpmbuild --exclude build --exclude bin --exclude tools -zcf \
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
   make
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   make doxygen-doc >/dev/null 2>&1 # only needed by some UI component
   make install
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   cd .. # from build to component root
   # set the .spec vars and run rpmbuild
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
   printf "[global] pkgversion=${version} " > setup.cfg 2>/dev/null
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

no_build()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   cd $COMPONENT
   mkdir -p rpmbuild/SOURCES rpmbuild/SPECS rpmbuild/SRPMS rpmbuild/BUILD rpmbuild/RPMS 2>/dev/null
   # creating binary tarball
   tar --exclude project --exclude rpmbuild \
      -czf rpmbuild/SOURCES/${PACKAGE_NAME}-$VERSION-$AGE.tar.gz *
   if [ $? -ne 0 ]; then
     echo ERROR
     exit
   fi
   eval "sed -e 's/__version__/$VERSION/g' -e 's/__release__/$AGE/g' \
      < project/$PACKAGE_NAME.spec.in > project/$PACKAGE_NAME.spec"
   rpmbuild -bb --target $ARCH --define "_topdir $BUILD_DIR/org.glite.wms/$COMPONENT/rpmbuild" \
      project/$PACKAGE_NAME.spec
   if [ $? -ne 0 ]; then
     echo ERROR
     exit
   fi
   cd $BUILD_DIR/org.glite.wms
}

rpmlint()
{
   /usr/bin/rpmlint --file=/usr/share/rpmlint/config -i $1
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
   sudo yum -y install rpmlint mod_fcgid mod_ssl gridsite-apache httpd-devel zlib-devel \
      boost-devel c-ares-devel glite-px-proxyrenewal-devel voms-devel voms-clients \
      argus-pep-api-c-devel lcmaps-without-gsi-devel lcmaps-devel classads-devel \
      glite-build-common-cpp gsoap-devel libtar-devel cmake globus-ftp-client \
      globus-ftp-client-devel log4cpp-devel log4cpp glite-jobid-api-c \
      glite-jobid-api-c-devel glite-jobid-api-cpp-devel openldap-devel python-ldap \
      glite-wms-utils-exception glite-wms-utils-classad \
      glite-wms-utils-exception-devel glite-wms-utils-classad-devel \
      chrpath cppunit-devel glite-jdl-api-cpp-devel glite-lb-client-devel \
      glite-lbjp-common-gsoap-plugin-devel condor-emi glite-ce-cream-client-api-c glite-ce-cream-client-devel
}

if [ -z $6 ]; then
   echo "wms <build-dir-name> <emi-release> <os> <want_external_deps> <want_vcs_checkout> <want_cleanup> (missing: <tag>)" # TODO tag
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
# TODO
#if [ $PLATFORM == "deb" ]; then
#   PACKAGER=rpm
#else
#   PACKAGER=deb
#fi
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

COMPONENT=( org.glite.wms.common org.glite.wms.ism org.glite.wms.helper org.glite.wms.purger org.glite.wms.jobsubmission org.glite.wms.manager org.glite.wms.wmproxy org.glite.wms.ice org.glite.wms.nagios org.glite.wms org.glite.wms.brokerinfo-access org.glite.wms.wmproxy-api-cpp org.glite.wms.wmproxy-api-java org.glite.wms.wmproxy-api-python org.glite.wms-ui.api-python org.glite.wms-ui.commands )
BUILD_TYPE=( autotools autotools autotools autotools autotools autotools autotools autotools null pkg_only autotools autotools ant python autotools autotools )
PACKAGE_NAME=( glite-wms-common glite-wms-ism glite-wms-helper glite-wms-purger glite-wms-jobsubmission glite-wms-manager glite-wms-wmproxy glite-wms-ice emi-wms-nagios emi-wms glite-wms-brokerinfo-access glite-wms-wmproxy-api-cpp glite-wms-wmproxy-api-java glite-wms-wmproxy-api-python glite-wms-ui-api-python glite-wms-ui-commands )
VERSION=( 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 )
AGE=( 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 )
START=8
END=15

for i in `seq 0 $((START - 1))`; do
   TEMP="$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}/tmp"
   export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$TEMP/$LOCAL_PKGCFG_LIB
done
for i in `seq $START $END`; do
   echo -e "\n*** building component ${COMPONENT[$i]} ***\n"
   
   if [ $6 -ne 0 ]; then
      cd "$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}"
      echo -e "\n*** cleaning up ${COMPONENT[$i]} ***\n"
      make -C build clean 2>/dev/null
      rm -rf rpmbuild tgz RPMS 2>/dev/null
      continue
   fi

   TEMP="$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}/tmp"
   export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$TEMP/$LOCAL_PKGCFG_LIB
   case ${BUILD_TYPE[$i]} in
     "autotools" )
         autotools_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} $TEMP
         ;;
     "cmake" )
         ant_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} $TEMP
         ;;
     "ant" )
         ant_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} $TEMP
         ;;
     "python" )
         python_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} $TEMP
         ;;
     "null" )
         echo "Skipping  ${COMPONENT[$i]}"
         ;;
     "pkg_only" )
         no_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]}
         ;;
     * )
         echo "Build type not found for ${COMPONENT[$i]}"
         exit
         ;;
   esac
done
