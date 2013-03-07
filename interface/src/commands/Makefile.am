## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = \
	libglite_wms_wmproxy_commands.la \
	libglite_wms_wmproxy_pipe.la


noinst_HEADERS = \
	listfiles.h \
	listjobmatch.h \
        MatchingPipe_nb.h \
	MatchingPipe.h

libglite_wms_wmproxy_commands_la_SOURCES = \
	listfiles.cpp \
	listjobmatch.cpp

libglite_wms_wmproxy_pipe_la_SOURCES = \
        MatchingPipe_nb.cpp \
	MatchingPipe.cpp

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	$(GLITE_WMS_COMMON_LOGGER_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS) \
	-DSHOW_EXECUTION

MAINTAINERCLEANFILES = Makefile.in


