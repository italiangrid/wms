#!/bin/sh
\rm -f /tmp/glite-wms-server-$1-$2.$3.tar.gz

tar czvf /tmp/glite-wms-server-$1-$2.$3.tar.gz \
            --exclude CMakeCache.txt \
            --exclude cmake_install.cmake \
            --exclude "*$4*" \
            --exclude "*Makefile" \
            --exclude "*CMakeFiles*" \
            --exclude install_manifest.txt \
            --exclude "*libglite*.so*" \
            --exclude "*pkgconfig/wms-server.pc" \
            --exclude "glite-wms-check-daemons.sh" \
            --exclude "glite-wms-parse-configuration.sh" \
            --exclude "config/glite-wms-ice" \
            --exclude "config/glite-wms-clean-lm-recycle.sh" \
            --exclude "config/glite-wms-jc" \
            --exclude "config/glite-wms-lm" \
            --exclude "*/.git*" \
            --exclude "rpmbuild*" \
	    --exclude "usr*" \
	    --exclude "opt*" \
	    --exclude "etc*" \
            -C $5 \
            .
