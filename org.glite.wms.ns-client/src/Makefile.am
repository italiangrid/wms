
## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wms_manager_ns_client.la

nsclientdir = $(includedir)/glite/wms/ns-client

nsclient_HEADERS = \
	common.h \
	exceptions.h \
	notification.h \
	NSClient.h \
	NSClientInterface.h

libglite_wms_manager_ns_client_la_SOURCES = NSClient.cpp \
	common.cpp

libglite_wms_manager_ns_client_la_LIBADD = \
        $(GLITE_WMS_NS_COMMON_CLIENT_COMMANDS_LIBS) 
#	-L$(top_srcdir)/../../org.glite.wms.ns-common/build/src/libnsclientcommands.la 

AM_CPPFLAGS = -I$(top_srcdir)/src \
        $(GLITE_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(GLOBUS_THR_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in

#	$(GLITE_CFLAGS)/glite/wms/ns-common 
#	-I$(top_srcdir)/../org.glite.wms.ns-common/src/commands
#	-I$(top_srcdir)/../org.glite.wms.ns-common/src/fsm
#	-I$(top_srcdir)/../org.glite.wms.ns-common/src/versions 
