## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wms_gangmatch_classad_plugin.la
 
libglite_wms_gangmatch_classad_plugin_la_SOURCES = \
	gangmatch_classad_plugin.cpp \
	doMatch.cpp \
	retrieveCloseSEsInfo.cpp

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
        $(GLIRW_WMS_ISM) \
	$(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS)
 
MAINTAINERCLEANFILES = Makefile.in
