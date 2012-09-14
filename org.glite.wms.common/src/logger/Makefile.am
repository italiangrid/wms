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
	manipulators_ts.cpp \
	key_value.cpp \
	logging.cpp
libglite_wms_logger_la_LIBADD = \
	-llog4cpp \
	$(BOOST_REGEX_LIBS) \
	$(BOOST_THREAD_LIBS) \
	$(BOOST_FS_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(BOOST_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~
