#!/bin/sh
CURPATH=`pwd`
\rm -rf build_tmp
mkdir -p build_tmp/DEBIAN
cp deb-control-file.txt build_tmp/DEBIAN/control
mkdir -p build_tmp/usr
#echo cmake . -DPREFIX=${CURPATH}/build_tmp/usr -DOFFICIAL_INSTALL_PREFIX=/usr
cmake . -DPREFIX=${CURPATH}/build_tmp/usr -DOFFICIAL_INSTALL_PREFIX=/usr
make
make install
