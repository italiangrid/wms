#!/bin/bash

set +e

VERSION=$1
echo $VERSION
if [ "X$VERSION" == "X" ]; then 
VERSION="9.9.9"
fi
cores=`cat /proc/cpuinfo|grep processor|wc -l`

#
#
# Setting up workarea
#
#
cd $HOME
mkdir WORKAREA
cd WORKAREA

#
#
# Checking out software from GIT
#
#
git clone git@github.com:italiangrid/classad-utils.git
git clone git@github.com:italiangrid/jobman-exception.git
git clone git@github.com:MarcoCecchi/org.glite.wms.git

#
#
# Building and staging classad-utils
#
#
cd classad-utils
cmake . -DCMAKE_INSTALL_PREFIX=$HOME/WORKAREA/stage -Dprefix=$HOME/WORKAREA/stage/usr -Dexec_prefix=$HOME/WORKAREA/stage/usr -Dlibdir=$HOME/WORKAREA/stage/usr/lib -Dincludedir=$HOME/WORKAREA/stage/usr/include -DPVER=$VERSION
make -j$cores install
cd ..

#
#
# Building and staging jobman-exception
#
#
cd jobman-exception
cmake . -DCMAKE_INSTALL_PREFIX=$HOME/WORKAREA/stage  -Dprefix=$HOME/WORKAREA/stage/usr -Dexec_prefix=$HOME/WORKAREA/stage/usr -Dlibdir=$HOME/WORKAREA/stage/usr/lib -Dincludedir=$HOME/WORKAREA/stage/usr/include -DPVER=$VERSION
make -j$cores install
cd ..


#
#
#
#
#  C  O  M  M  O  N
#
#
#
#


#
#
# Building and staging org.glite.wms.common
#
#
mkdir -p $HOME/WORKAREA/debian/build_src/org.glite.wms.common/
tar czf $HOME/WORKAREA/debian/build_src/org.glite.wms.common/libglite-wms-common-src-$VERSION.orig.tar.gz -C $HOME/WORKAREA/org.glite.wms/org.glite.wms.common .
cd org.glite.wms/org.glite.wms.common
cmake . -DPREFIX=$HOME/WORKAREA/stage/usr -DINSTALL_BUILD_DEV=$HOME/WORKAREA/debian/build_dev/org.glite.wms.common/usr -DINSTALL_BUILD=$HOME/WORKAREA/debian/build_nodev/org.glite.wms.common/usr -DOFFICIAL_INSTALL_PREFIX=/usr -DPVER=$VERSION
make -j$cores install

#
#
# Preparing for debian packaging 
#
#
mkdir -p $HOME/WORKAREA/debian/build_nodev/org.glite.wms.common/DEBIAN
cp debian/deb-control-file.txt $HOME/WORKAREA/debian/build_nodev/org.glite.wms.common/DEBIAN/control
cp debian/libglite-wms-common.install $HOME/WORKAREA/debian/build_nodev/org.glite.wms.common/DEBIAN/

#
#
# Preparing for debian dev packaging
#
#
mkdir -p $HOME/WORKAREA/debian/build_dev/org.glite.wms.common/DEBIAN
cp debian/deb-control-file-dev.txt $HOME/WORKAREA/debian/build_dev/org.glite.wms.common/DEBIAN/control
cp debian/libglite-wms-common-dev.install $HOME/WORKAREA/debian/build_dev/org.glite.wms.common/DEBIAN/

#
#
# Creating debian packages
#
#
dpkg -b $HOME/WORKAREA/debian/build_nodev/org.glite.wms.common $HOME/WORKAREA/debian/build_nodev/org.glite.wms.common/glite-wms-common-$VERSION.deb
dpkg -b $HOME/WORKAREA/debian/build_dev/org.glite.wms.common $HOME/WORKAREA/debian/build_dev/org.glite.wms.common/glite-wms-common-dev-$VERSION.deb

#
#
# Creating tar.gz containing the binary artifacts and devel stuff
#
#
mkdir -p $HOME/WORKAREA/TGZ
tar czvf $HOME/WORKAREA/TGZ/glite-wms-common-$VERSION.tar.gz -C $HOME/WORKAREA/debian/build_nodev/org.glite.wms.common usr
tar czvf $HOME/WORKAREA/TGZ/glite-wms-common-dev-$VERSION.tar.gz -C $HOME/WORKAREA/debian/build_dev/org.glite.wms.common usr


#
# Preparing for debian src packaging
#
#
cd $HOME/WORKAREA/debian/build_src/org.glite.wms.common
tar zxf libglite-wms-common-src-$VERSION.orig.tar.gz 
#tar czvf libglite-wms-common-$VERSION-debian.tar.gz -C $HOME/WORKAREA/debian/build_src/org.glite.wms.common debian
cd $HOME
dpkg-source -c$HOME/WORKAREA/debian/build_src/org.glite.wms.common/debian/deb-control-file-src.txt -b $HOME/WORKAREA/debian/build_src/org.glite.wms.common
mkdir -p $HOME/WORKAREA/SOURCES
mkdir -p $HOME/WORKAREA/DEBS
tar czvf $HOME/WORKAREA/SOURCES/libglite-wms-common-$VERSION-debian.tar.gz -C $HOME/WORKAREA/debian/build_src/org.glite.wms.common debian
mv $HOME/libglite-wms-common-src_3.5.0-0.tar.gz $HOME/WORKAREA/SOURCES
mv $HOME/libglite-wms-common-src_3.5.0-0.dsc $HOME/WORKAREA/SOURCES
mv $HOME/WORKAREA/debian/build_nodev/org.glite.wms.common/*.deb $HOME/WORKAREA/DEBS
mv $HOME/WORKAREA/debian/build_dev/org.glite.wms.common/*.deb $HOME/WORKAREA/DEBS
mv $HOME/WORKAREA/debian/build_src/org.glite.wms.common/libglite-wms-common-src-$VERSION.orig.tar.gz $HOME/WORKAREA/SOURCES

#
#
#
#
#  I   S   M
#
#
#
#
mkdir -p $HOME/WORKAREA/debian/build_src/org.glite.wms.ism/
tar czf $HOME/WORKAREA/debian/build_src/org.glite.wms.ism/libglite-wms-ism-src-$VERSION.orig.tar.gz -C $HOME/WORKAREA/org.glite.wms/org.glite.wms.ism .
cd $HOME/WORKAREA/org.glite.wms/org.glite.wms.ism
cmake . -DPREFIX=$HOME/WORKAREA/stage/usr -DINSTALL_BUILD_DEV=$HOME/WORKAREA/debian/build_dev/org.glite.wms.ism/usr -DINSTALL_BUILD=$HOME/WORKAREA/debian/build_nodev/org.glite.wms.ism/usr -DOFFICIAL_INSTALL_PREFIX=/usr -DPVER=$VERSION
make -j$cores install

#
#
# Preparing for debian packaging
#
#
mkdir -p $HOME/WORKAREA/debian/build_nodev/org.glite.wms.ism/DEBIAN
cp debian/deb-control-file.txt $HOME/WORKAREA/debian/build_nodev/org.glite.wms.ism/DEBIAN/control
cp debian/libglite-wms-ism.install $HOME/WORKAREA/debian/build_nodev/org.glite.wms.ism/DEBIAN/

#
#
# Preparing for debian dev packaging
#
#
mkdir -p $HOME/WORKAREA/debian/build_dev/org.glite.wms.ism/DEBIAN
cp debian/deb-control-file-dev.txt $HOME/WORKAREA/debian/build_dev/org.glite.wms.ism/DEBIAN/control
cp debian/libglite-wms-ism-dev.install $HOME/WORKAREA/debian/build_dev/org.glite.wms.ism/DEBIAN/

#
#
# Creating debian packages
#
#
dpkg -b $HOME/WORKAREA/debian/build_nodev/org.glite.wms.ism $HOME/WORKAREA/debian/build_nodev/org.glite.wms.ism/glite-wms-ism-$VERSION.deb
dpkg -b $HOME/WORKAREA/debian/build_dev/org.glite.wms.ism $HOME/WORKAREA/debian/build_dev/org.glite.wms.ism/glite-wms-ism-dev-$VERSION.deb


mv $HOME/WORKAREA/debian/build_nodev/org.glite.wms.ism/*.deb $HOME/WORKAREA/DEBS
mv $HOME/WORKAREA/debian/build_dev/org.glite.wms.ism/*.deb $HOME/WORKAREA/DEBS
