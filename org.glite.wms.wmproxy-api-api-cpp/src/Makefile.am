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



wm.h : $(GLITE_WMS_WSDL_PATH)/WMProxy.wsdl
	@#Create the files
	@echo "wsdl2h parsing .wsdl"
	$(GSOAP_LOCATION)/bin/wsdl2h -o wm.h \
	-I$(GSOAP_LOCATION)/include \
	-t $(GLITE_WMS_WSDL_PATH)/wmptypemap.dat \
	$(GLITE_WMS_WSDL_PATH)/WMProxy.wsdl

soapC.cpp : wm.h
	@#Create the files
	@echo "gsoap compiling .h"
	$(GSOAP_LOCATION)/bin/soapcpp2  -I$(GSOAP_LOCATION)/include:$(GSOAP_LOCATION)/import:$(GSOAP_LOCATION)/share/gsoap/import wm.h

soapClient.cpp : soapC.cpp

lib_LTLIBRARIES = libglite_wms_wmproxy_api_cpp.la

libglite_wms_wmproxy_api_cpp_la_SOURCES = \
	wm.h \
	soapC.cpp \
	soapClient.cpp \
	wmproxy_api.cpp \
	wmproxy_api_utilities.cpp \
	dom.cpp

libglite_wms_wmproxy_api_cpp_la_LIBADD = $(GRIDSITE_OPENSSL_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GSOAP_CFLAGS) \
	$(GRIDSITE_OPENSSL_CFLAGS) \
	-I$(top_srcdir)/interface \
	-DWITH_OPENSSL \
	-DWITH_IPV6 \
	-DUSE_GSOAP_PARSER \
	-DGSOAP_VERSION_NUM=$(GSOAP_VERSION_NUM)
	-UHAVE_CONFIG_H
	
	
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wmproxy-api-cpp.pc

MAINTAINERCLEANFILES = Makefile.in

