#!/bin/bash

create_source_tarball()
{
   mkdir -p rpmbuild/SOURCES 2>/dev/null
   tar --exclude reports --exclude rpmbuild --exclude build --exclude bin \
     --exclude tools --exclude CMakeFiles --exclude CMakeCache.txt \
     --exclude cmake_install.cmake -zcf ./rpmbuild/SOURCES/$1-$2-$3.$4.tar.gz .
   if [ $? -ne 0 ]; then
     echo ERROR creating tarball
     exit
   fi
}

autotools_build()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   LOCAL_STAGE_DIR=$5

   mkdir -p build src/autogen 2>/dev/null
   cp -R $BUILD_DIR/org.glite.wms/org.glite.wms-ui/project/doxygen project/doxygen 2>/dev/null # needed by some UI component
   aclocal -I ${M4_LOCATION} && \
	libtoolize --force && autoheader && automake --foreign --add-missing --copy && \
	autoconf
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   create_source_tarball ${PACKAGE_NAME} ${VERSION} ${AGE} ${PLATFORM}
   cd build
   ../configure --prefix=${LOCAL_STAGE_DIR}/usr --sysconfdir=${LOCAL_STAGE_DIR}/etc --disable-static PVER=${VERSION}
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   make -j$CORES
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
   mkdir -p ${LOCAL_STAGE_DIR}/usr/share/doc/${PACKAGE_NAME}
   cp -r autodoc/html ${LOCAL_STAGE_DIR}/usr/share/doc/${PACKAGE_NAME} 2>/dev/null # needed by some UI component
   cd .. # from build to component root
   if [ $PACKAGER = "rpm" ]; then
      rpm_package $VERSION $AGE $PLATFORM $PACKAGE_NAME $COMPONENT $LOCAL_STAGE_DIR
   elif [ $PACKAGER = "deb" ]; then
      deb_package $VERSION $AGE $PLATFORM $PACKAGE_NAME $COMPONENT $LOCAL_STAGE_DIR
   fi
   mv ./rpmbuild/SOURCES/${PACKAGE_NAME}-${VERSION}-${AGE}.${PLATFORM}.tar.gz "$BUILD_DIR"/org.glite.wms/tgz
}

cmake_build()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   LOCAL_STAGE_DIR=$5

   create_source_tarball ${PACKAGE_NAME} ${VERSION} ${AGE} ${PLATFORM}

   cmake -DPREFIX:string=$LOCAL_STAGE_DIR/usr -DPVER:string=$VERSION .

   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   make install
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   if [ $PACKAGER = "rpm" ]; then
      rpm_package $VERSION $AGE $PLATFORM $PACKAGE_NAME $COMPONENT $LOCAL_STAGE_DIR
   elif [ $PACKAGER = "deb" ]; then
      deb_package $VERSION $AGE $PLATFORM $PACKAGE_NAME $COMPONENT $LOCAL_STAGE_DIR
   fi
   mv ./rpmbuild/SOURCES/${PACKAGE_NAME}-${VERSION}-${AGE}.${PLATFORM}.tar.gz "$BUILD_DIR"/org.glite.wms/tgz
}

ant_build()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   LOCAL_STAGE_DIR=$5
   PLATFORM=noarch # that's supposed to be java stuff

   mkdir -p lib bin autogen doc/autogen src/autogen ${LOCAL_STAGE_DIR}/usr/share/doc/
   create_source_tarball ${PACKAGE_NAME} ${VERSION} ${AGE} ${PLATFORM}
   echo "dist.location=${LOCAL_STAGE_DIR}" > .configuration.properties
   echo "org.glite.wms.wsdl.location=$BUILD_DIR/org.glite.wms/org.glite.wms.interface/src/server" >> .configuration.properties
   echo "module.version=${VERSION}" >> .configuration.properties
   ant
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   ant install
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   if [ $PACKAGER = "rpm" ]; then
      rpm_package $VERSION $AGE $PLATFORM $PACKAGE_NAME $COMPONENT $LOCAL_STAGE_DIR
   elif [ $PACKAGER = "deb" ]; then
      deb_package $VERSION $AGE $PLATFORM $PACKAGE_NAME $COMPONENT $LOCAL_STAGE_DIR
   fi
   mv ./rpmbuild/SOURCES/${PACKAGE_NAME}-${VERSION}-${AGE}.${PLATFORM}.tar.gz "$BUILD_DIR"/org.glite.wms/tgz
}

python_build()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   LOCAL_STAGE_DIR=$5

   create_source_tarball ${PACKAGE_NAME} ${VERSION} ${AGE} ${PLATFORM}
   echo "[global]" > setup.cfg 2>/dev/null
   echo "pkgversion=${VERSION}" >> setup.cfg 2>/dev/null
   python setup.py install -O1 --prefix ${LOCAL_STAGE_DIR}/usr --install-data ${LOCAL_STAGE_DIR}
   if [ $PACKAGER = "rpm" ]; then
      rpm_package $VERSION $AGE $PLATFORM $PACKAGE_NAME $COMPONENT $LOCAL_STAGE_DIR
   elif [ $PACKAGER = "deb" ]; then
      deb_package $VERSION $AGE $PLATFORM $PACKAGE_NAME $COMPONENT $LOCAL_STAGE_DIR
   fi
   mv ./rpmbuild/SOURCES/${PACKAGE_NAME}-${VERSION}-${AGE}.$PLATFORM.tar.gz "$BUILD_DIR"/org.glite.wms/tgz
}

mp_build()
{
   COMPONENT=$1
   VERSION=$2
   AGE=$3
   PACKAGE_NAME=$4
   PLATFORM=noarch

   mkdir -p rpmbuild/SOURCES rpmbuild/BUILD 2>/dev/null rpmbuild/SPECS rpmbuild/SRPMS rpmbuild/BUILD rpmbuild/RPMS 2>/dev/null
   create_source_tarball ${PACKAGE_NAME} ${VERSION} ${AGE} ${PLATFORM}
   if [ $? -ne 0 ]; then
     echo ERROR creating tarball
     exit
   fi
   eval "sed -e 's/__version__/$VERSION/g' -e 's/__release__/$AGE/g' \
      < project/$PACKAGE_NAME.spec.in > project/$PACKAGE_NAME.spec"
   rpmbuild -ba --define "_topdir $BUILD_DIR/org.glite.wms/$COMPONENT/rpmbuild" project/$PACKAGE_NAME.spec
   if [ $? -ne 0 ]; then
     echo ERROR
     exit
   fi
   mv rpmbuild/SOURCES/${PACKAGE_NAME}-${VERSION}-${AGE}.noarch.tar.gz "$BUILD_DIR"/org.glite.wms/tgz
   mv rpmbuild/RPMS/noarch/* "$BUILD_DIR"/org.glite.wms/RPMS
   mv rpmbuild/SRPMS/* "$BUILD_DIR"/org.glite.wms/SRPMS
}

rpm_package()
{
   VERSION=$1
   AGE=$2
   PLATFORM=$3
   PACKAGE_NAME=$4
   COMPONENT=$5
   LOCAL_STAGE_DIR=$6

   mkdir rpmbuild/SPECS rpmbuild/SRPMS rpmbuild/BUILD rpmbuild/RPMS 2>/dev/null
   eval "sed -e 's/%{extversion}/$VERSION/g' -e 's/%{extage}/$AGE/g' \
      -e 's/%{extdist}/$PLATFORM/g' -e 's/%{extcdate}/`date +'%a %b %d %Y'`/g' \
      -e 's/%{extclog}/Bug fixing/g' \
      < project/$PACKAGE_NAME.spec > rpmbuild/SPECS/${PACKAGE_NAME}.spec"
   rpmbuild -ba --define "_topdir ${BUILD_DIR}/org.glite.wms/$COMPONENT/rpmbuild" \
      --define "extbuilddir $LOCAL_STAGE_DIR" rpmbuild/SPECS/${PACKAGE_NAME}.spec
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi

   /usr/bin/rpmlint --file=/usr/share/rpmlint/config -i rpmbuild/RPMS/$ARCH/*
   /usr/bin/rpmlint --file=/usr/share/rpmlint/config -i rpmbuild/SRPMS/*

   mv rpmbuild/RPMS/$ARCH/* "$BUILD_DIR"/org.glite.wms/RPMS
   mv rpmbuild/RPMS/noarch/* "$BUILD_DIR"/org.glite.wms/RPMS 2>/dev/null
   mv rpmbuild/SRPMS/* "$BUILD_DIR"/org.glite.wms/SRPMS
}

deb_package()
{
   VERSION=$1
   AGE=$2
   PLATFORM=$3
   PACKAGE_NAME=$4
   COMPONENT=$5
   LOCAL_STAGE_DIR=$6

   # TODO to be tested
   # TODO generation of src-deb missing
   mkdir -p debian/nodev/ 2>/dev/null
   cp debian/deb-control-file.txt debian/build_nodev/debian/control
   cp debian/lib$PACKAGE_NAME.install debian/nodev/debian
   mkdir -p debian/build_dev/org.glite.wms.common/debian
   cp debian/deb-control-file-dev.txt debian/dev/debian/control
   cp debian/libglite-wms-common-dev.install debian/de/debian
   dpkg -b debian/nodev/ debian/nodev/$PACKAGE_NAME-$VERSION.deb
   dpkg -b debian/dev/ debian/dev/$PACKAGE_NAME-dev-$VERSION.deb
}

get_external_deps()
{
   if [ $PACKAGER = "rpm" ]; then
      # install EPEL repositories
      if [ $PLATFORM = "sl6" ]; then
        sudo rpm -ivh http://ftp.tu-chemnitz.de/pub/linux/fedora-epel/6/i386/epel-release-6-7.noarch.rpm
      elif [ $PLATFORM = "sl5" ]; then
         sudo rpm -ivh http://fedora.uib.no/epel/5/i386/epel-release-5-4.noarch.rpm
      fi
      # install EMI repositories
      sudo rpm --import http://emisoft.web.cern.ch/emisoft/dist/EMI/$EMI_RELEASE/RPM-GPG-KEY-emi
      sudo rpm -ivh "http://emisoft.web.cern.ch/emisoft/dist/EMI/$EMI_RELEASE/$PLATFORM/x86_64/base/emi-release-${EMI_RELEASE}.0.0-1.$PLATFORM.noarch.rpm"
      # WMS build dependencies and all that's needed to build and package
      sudo yum -y install ${RH_DEPS_LIST[@]}
   elif [ $PACKAGER = "deb" ]; then
      # TODO: get extra O.S. and EMI repositories
      sudo apt-get -y install ${DEB_DEPS_LIST[@]}
   fi
   sudo yum -y install ${INT_DEPS_LIST[@]}
}

#
# MAIN
#
if [ -z $8 ]; then # let's keep start and end optional
   echo "wms <tag:master/...> <build-dir-name> <emi-release:1/2/3> <os:sl5/sl6/deb6> <want_external_deps:0/1> <want_vcs_checkout:0/1> <unused:0/1> <want_mock:0/1> <<start:0-16>> <<end::0-16>>"
   exit
fi

BUILD_DIR=`pwd`/$2
EMI_RELEASE=$3
PLATFORM=$4
if [ ${PLATFORM:0:3} = "deb" ]; then
   PACKAGER=deb
elif [ ${PLATFORM:0:2} = "sl" ]; then
   PACKAGER=rpm
fi
M4_LOCATION=/usr/share/emi/build/m4
ARCH=`uname -i`
CORES=`cat /proc/cpuinfo|grep processor|wc -l`

RH_DEPS_LIST=( ant bouncycastle doxygen docbook-style-xsl libxslt-devel gcc gcc-c++ python-devel SOAPpy PyXML python-fpconst libtool automake swig yum-priorities pkgconfig mock rpm-build rpmlint git mod_fcgid fcgi-devel mod_ssl axis2 gridsite-devel httpd-devel zlib-devel boost-devel c-ares-devel gsoap-devel libtar-devel cmake openldap-devel python-ldap globus-ftp-client globus-ftp-client-devel log4cpp-devel log4cpp globus-gram-protocol-devel myproxy-devel expat expat-devel fcgi-devel fcgi libtar libtar-devel httpd-devel myproxy-devel cmake ant axis2 bouncycastle.noarch python-fpconst PyXML SOAPpy )
INT_DEPS_LIST=( glite-jobid-api-c glite-jobid-api-c-devel glite-jobid-api-cpp-devel glite-px-proxyrenewal-devel voms-devel voms-clients argus-pep-api-c-devel lcmaps-without-gsi-devel lcmaps-devel classads-devel glite-build-common-cpp glite-wms-utils-exception glite-wms-utils-classad glite-wms-utils-exception-devel glite-wms-utils-classad-devel chrpath cppunit-devel glite-jdl-api-cpp-devel glite-lb-client-devel glite-lbjp-common-gsoap-plugin-devel condor-emi glite-ce-cream-client-api-c glite-ce-cream-client-devel emi-trustmanager emi-trustmanager-axis )
DEB_DEPS_LIST=( )
COMPONENT=( org.glite.wms.configuration org.glite.wms.common org.glite.wms.purger org.glite.wms.core org.glite.wms.jobsubmission org.glite.wms.interface org.glite.wms.ice org.glite.wms.nagios org.glite.wms org.glite.wms.brokerinfo-access org.glite.wms.wmproxy-api-cpp org.glite.wms.wmproxy-api-java org.glite.wms.wmproxy-api-python org.glite.wms-ui.api-python org.glite.wms-ui.commands )
BUILD_TYPE=( autotools autotools autotools autotools autotools autotools autotools null metapackage cmake cmake ant python cmake cmake )
PACKAGE_NAME=( glite-wms-configuration glite-wms-common glite-wms-purger glite-wms-jobsubmission glite-wms-core glite-wms-interface glite-wms-ice emi-wms-nagios emi-wms glite-wms-brokerinfo-access glite-wms-wmproxy-api-cpp glite-wms-wmproxy-api-java glite-wms-wmproxy-api-python glite-wms-ui-api-python glite-wms-ui-commands )
VERSION=( 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 3.5.0 )
AGE=( 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 )
START=$9
END=${10}

# mock build
if [ $8 -eq 1 ]; then
   echo -e "\n*** mock build ***\n"

   # file /etc/mock/emi${EMI_RELEASE}-$PLATFORM-$ARCH.cfg must be created, with all the required repositories and stuff
   if [ ! -r /etc/mock/emi${EMI_RELEASE}-$PLATFORM-$ARCH.cfg ]; then
      echo "Expected configuration emi${EMI_RELEASE}-$PLATFORM-$ARCH.cfg does not exists. Do you also want me to prepare the mock configuration?!? What else?"
      exit
   fi

   if [ $5 -eq 1 ]; then
      echo -e "\n*** wiping out the mock environment ***\n"
      mock -r emi${EMI_RELEASE}-$PLATFORM-$ARCH --clean
      echo -e "\n*** one time initialization of the mock environment ***\n"
      mock -r emi${EMI_RELEASE}-$PLATFORM-$ARCH --init
      #yum --installroot /etc/mock/emi${EMI_RELEASE}-$PLATFORM-$ARCH clean all
      #yum -y --nogpgcheck --installroot /etc/mock/emi${EMI_RELEASE}-$PLATFORM-$ARCH install ${RH_DEPS_LIST[@]}
   fi

   rm -f /var/lib/mock/emi${EMI_RELEASE}-$PLATFORM-$ARCH/result/build.log 
   for i in `seq $START $END`; do
      if [ `expr match "${PACKAGE_NAME[$i]}" '.*java.*'` -gt 0 ]; then
         ARTEFACT_ARCH=noarch
         mock -r emi${EMI_RELEASE}-$PLATFORM-$ARCH --no-clean --rebuild \
            "$BUILD_DIR/org.glite.wms/SRPMS/${PACKAGE_NAME[$i]}-${VERSION}-${AGE}.${ARTEFACT_ARCH}.src.rpm"
      else
         ARTEFACT_ARCH=$ARCH
         mock -r emi${EMI_RELEASE}-$PLATFORM-$ARCH --no-clean --rebuild \
            "$BUILD_DIR/org.glite.wms/SRPMS/${PACKAGE_NAME[$i]}-${VERSION}-${AGE}.${PLATFORM}.src.rpm"
      fi
      if [ $? -ne 0 ]; then
         echo ERROR
         exit
      fi
      # install the generated package(s)
      if [ -r /var/lib/mock/emi${EMI_RELEASE}-$PLATFORM-$ARCH/result/${PACKAGE_NAME[$i]}-lib-$VERSION-$AGE.$PLATFORM.$ARTEFACT_ARCH.rpm ]; then
         mock -r emi${EMI_RELEASE}-$PLATFORM-$ARCH --install \
            /var/lib/mock/emi${EMI_RELEASE}-$PLATFORM-$ARCH/result/${PACKAGE_NAME[$i]}-lib-$VERSION-$AGE.$PLATFORM.$ARTEFACT_ARCH.rpm
      fi
      if [ -r /var/lib/mock/emi${EMI_RELEASE}-$PLATFORM-$ARCH/result/${PACKAGE_NAME[$i]}-devel-$VERSION-$AGE.$PLATFORM.$ARTEFACT_ARCH.rpm ]; then
         mock -r emi${EMI_RELEASE}-$PLATFORM-$ARCH --install \
            /var/lib/mock/emi${EMI_RELEASE}-$PLATFORM-$ARCH/result/${PACKAGE_NAME[$i]}-devel-$VERSION-$AGE.$PLATFORM.$ARTEFACT_ARCH.rpm
      fi
      mock -r emi${EMI_RELEASE}-$PLATFORM-$ARCH --install \
         /var/lib/mock/emi${EMI_RELEASE}-$PLATFORM-$ARCH/result/${PACKAGE_NAME[$i]}-$VERSION-$AGE.$PLATFORM.$ARTEFACT_ARCH.rpm
   done

   echo -e "\n*** mock build completed ***\n"
   exit
fi

if [ $5 -eq 1 ]; then
   echo -e "\n*** installing external dependencies ***\n"
   get_external_deps
fi

echo -e "\n*** build dir: $BUILD_DIR, platform: $PLATFORM, architecture: $ARCH ***\n"

if [ $6 -ne 0 ]; then
   sudo rm -rf "$BUILD_DIR"
   mkdir -p "$BUILD_DIR"
   cd "$BUILD_DIR"
   echo -e "\n*** checking out the WMS project ***\n"
   git clone --progress -v git@github.com:italiangrid/org.glite.wms.git
   if [ $? -ne 0 ]; then
      echo ERROR
      exit
   fi
   git checkout $1
   cd org.glite.wms
else
   cd "$BUILD_DIR"/org.glite.wms
   echo -e "\n*** NOT checking out the WMS project ***\n"
fi
mkdir tgz RPMS SRPMS 2>/dev/null

echo -e "\n*** starting build ***\n"

export PKG_CONFIG_PATH=$BUILD_DIR/org.glite.wms/org.glite.wms.jobsubmission/project/ # for emi-condorg.pc
if [ -d /usr/lib64 ]; then
   LOCAL_PKGCFG_LIB=usr/lib64/pkgconfig/
else
   LOCAL_PKGCFG_LIB=usr/lib/pkgconfig/
fi
for i in `seq 0 $((START - 1))`; do
   STAGE="$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}/stage"
   export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$STAGE/$LOCAL_PKGCFG_LIB
done
if [ $START -gt ${#COMPONENT[@]} ]; then
   echo component indices out of range
   exit
fi
for i in `seq $START $END`; do
   echo -e "\n*** building component ${COMPONENT[$i]} ***\n"
   
   echo -e "\n*** cleaning up ${COMPONENT[$i]} ***\n"
   cd "$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}"
   make -C build clean 2>/dev/null
   ant clean 2>/dev/null
   python setup.py clean --all 2>/dev/null
   rm -rf rpmbuild RPMS stage 2>/dev/null

   # hack required to integrate not os provided gsoap
   if [ ${COMPONENT[$i]} = "org.glite.wms.interface" ]; then
      if [ $PLATFORM = "sl6" ]; then
         ln -sf "$BUILD_DIR/org.glite.wms/org.glite.wms.interface/src/server/stdsoap2-2_7_16.cpp" \
            "$BUILD_DIR/org.glite.wms/org.glite.wms.interface/src/server/stdsoap2.cpp"
      elif [ $PLATFORM = "sl5" ]; then 
         ln -sf "$BUILD_DIR/org.glite.wms/org.glite.wms.interface/src/server/stdsoap2-2_7_13.cpp" \
            "$BUILD_DIR/org.glite.wms/org.glite.wms.interface/src/server/stdsoap2.cpp"
      fi
   fi

   STAGE="$BUILD_DIR/org.glite.wms/${COMPONENT[$i]}/stage"
   case ${BUILD_TYPE[$i]} in
     "autotools" )
         autotools_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} $STAGE
         ;;
     "cmake" )
         cmake_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} $STAGE
         ;;
     "ant" )
         ant_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} $STAGE
         ;;
     "python" )
         python_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]} $STAGE
         ;;
     "null" )
         echo "Skipping  ${COMPONENT[$i]}"
         ;;
     "metapackage" )
         mp_build ${COMPONENT[$i]} ${VERSION[$i]} ${AGE[$i]} ${PACKAGE_NAME[$i]}
         ;;
     * )
         echo "Build type not found for ${COMPONENT[$i]}"
         exit
         ;;
   esac
   cd $BUILD_DIR/org.glite.wms

   # the rpm cannot be created from a common stage dir, so why
   # should we have one at all? each 'tmp' dir can be a stage
   export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$STAGE/$LOCAL_PKGCFG_LIB
done

echo -e "\n*** native build completed ***\n"
