## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

SUBDIRS = DLI
	

lib_LTLIBRARIES = \
	libglite_wms_replicaservice.la


libglite_wms_replicaservice_la_SOURCES = \
	ReplicaService.cpp \
	ReplicaServiceException.cpp \
	catalogs_access_utils.cpp

AM_CPPFLAGS = \
	-I$(top_srcdir)/interface \
        -I$(top_srcdir)/src \
	-I$(top_srcdir)/src/DLI \
	$(GLITE_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	$(GSOAP_CFLAGS) \
	$(CLASSAD_CFLAGS)\
	$(GLITE_WMS_COMMON_SYSLOG_CFLAGS)


MAINTAINERCLEANFILES = Makefile.in *.*~
