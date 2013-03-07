## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wms_logger.la

libglite_wms_logger_la_SOURCES = \
	common.cpp \
	logbuf.cpp \
	logstream.cpp \
	manipulators.cpp \
	logstream_ts.cpp \
	container_ts.cpp \
	manipulators_ts.cpp

libglite_wms_logger_la_LIBADD = $(BOOST_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(BOOST_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~

