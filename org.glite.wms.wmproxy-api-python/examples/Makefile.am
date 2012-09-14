#
#       Copyright (c) 2004 on behalf of the EU EGEE Project:
#       The European Organization for Nuclear Research (CERN),
#       Istituto Nazionale di Fisica Nucleare (INFN), Italy
#       Datamat Spa, Italy
#       Centre National de la Recherche Scientifique (CNRS), France
#       CS Systeme d'Information (CSSI), France
#       Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
#       Universiteit van Amsterdam (UvA), Netherlands
#       University of Helsinki (UH.HIP), Finland
#       University of Bergen (UiB), Norway
#       Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom
#
#       Authors: Luca Petronzio <luca.petronzio@elsagdatamat.com>
#

WMPROXY_UTILS = $(top_builddir)/examples/libmake_proxy_cert.la

bin_PROGRAMS = \
	glite-wms-proxy-sign

glite_wms_proxy_sign_SOURCES = \
	make_proxy_cert.c
glite_wms_proxy_sign_LDFLAGS = --cflags -Xlinker --export-dynamic
glite_wms_proxy_sign_LDADD = \
	$(GRIDSITE_OPENSSL_LIBS) 

AM_CFLAGS = -I$(top_srcdir)/examples \
	-I$(top_srcdir)/build/examples \
	$(GRIDSITE_OPENSSL_CFLAGS) \
	-DWANT_NAMESPACES




MAINTAINERCLEANFILES = Makefile.in
