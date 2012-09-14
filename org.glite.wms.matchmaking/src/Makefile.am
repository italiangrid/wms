## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wms_matchmaking.la

matchmakingdir = $(includedir)/glite/wms/matchmaking
matchmaking_HEADERS = \
	utility.h \
	matchmakerISMImpl.h \
        exceptions.h \
        jdl_attributes.h \
        glue_attributes.h

libglite_wms_matchmaking_la_LDFLAGS = -all-static

libglite_wms_matchmaking_la_SOURCES = \
        matchmaker.cpp \
	matchmakerISMImpl.cpp \
	exceptions.cpp 


AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/interface \
	$(GLITE_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in
