## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wms_process.la

libglite_wms_process_la_SOURCES = \
	process.cpp \
	subprocess.cpp \
	user.cpp

libglite_wms_process_la_LIBADD = $(BOOST_LIBS)

processincludedir = $(includedir)/glite/wms/common/process
processinclude_HEADERS = \
	process.h \
	subprocess.h \
	user.h

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(BOOST_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~
