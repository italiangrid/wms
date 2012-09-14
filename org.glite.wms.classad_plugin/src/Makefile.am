## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

## Subdirectories list
SUBDIRS = gangmatch lb_rank fqan

lib_LTLIBRARIES = libglite_wms_classad_plugin_loader.la

classad_plugindir = $(includedir)/glite/wms/classad_plugin
classad_plugin_HEADERS = \
	classad_plugin_loader.h
 
libglite_wms_classad_plugin_loader_la_SOURCES = \
	classad_plugin_loader.cpp

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS)
 
MAINTAINERCLEANFILES = Makefile.in
