## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

noinst_LTLIBRARIES = \
	libglite_wms_client_delegateproxy.la \
	libglite_wms_client_jobinfo.la \
	libglite_wms_client_jobsubmit.la \
	libglite_wms_client_joboutput.la \
	libglite_wms_client_jobcancel.la \
	libglite_wms_client_joblistmatch.la	\
	libglite_wms_client_jobperusal.la
	
noinst_HEADERS     = \
	delegateproxy.h \
	jobinfo.h \
	jobcancel.h \
	joblistmatch.h \
	joboutput.h \
	jobsubmit.h \
	job.h \
	lbapi.h \
	listener.h


libglite_wms_client_delegateproxy_la_SOURCES = \
	delegateproxy.cpp \
	job.cpp

libglite_wms_client_jobinfo_la_SOURCES = \
	jobinfo.cpp \
	lbapi.cpp \
	job.cpp

#libglite_wms_client_jobattach_la_SOURCES = \
#	jobattach.cpp \
#	lbapi.cpp \
#	job.cpp

libglite_wms_client_jobsubmit_la_SOURCES = \
	jobsubmit.cpp \
	lbapi.cpp \
	job.cpp \
	listener.cpp

libglite_wms_client_joboutput_la_SOURCES = \
	joboutput.cpp \
	lbapi.cpp \
	job.cpp

libglite_wms_client_jobcancel_la_SOURCES = \
	jobcancel.cpp \
	lbapi.cpp \
	job.cpp

libglite_wms_client_joblistmatch_la_SOURCES = \
	joblistmatch.cpp \
	job.cpp

libglite_wms_client_jobperusal_la_SOURCES = \
	jobperusal.cpp \
	lbapi.cpp \
	job.cpp

AM_CPPFLAGS = \
	$(GLITE_CFLAGS) \
        -I$(top_srcdir)/src \
        -I$(top_srcdir)/build/src \
	$(GLOBUS_NOTHR_CFLAGS) \
	$(GRIDSITE_GLOBUS_CFLAGS) \
	$(GRIDSITE_GLOBUS_OPENSSL_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(LIBTAR_CFLAGS) \
	$(ZLIB_CFLAGS)
	
MAINTAINERCLEANFILES = Makefile.in

