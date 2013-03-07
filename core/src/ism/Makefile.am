## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

SUBDIRS = purchaser

lib_LTLIBRARIES    = \
	libglite_wms_ism.la 

libglite_wms_ism_la_SOURCES = \
        ism.cpp 

libglite_wms_ism_la_LIBADD = $(GLITE_WMS_COMMON_LOGGER_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(BOOST_LIBS) \
        $(CLASSAD_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(GLITE_WMS_COMMON_LOGGER_CFLAGS) \
	$(GLITE_WMS_COMMON_CONF_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *.*~
