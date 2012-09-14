## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wmsutils_jobid.la libglite_wmsutils_cjobid.la

libglite_wmsutils_jobid_la_SOURCES = \
	JobId.cpp \
	JobIdExceptions.cpp \
	manipulation.cpp 

libglite_wmsutils_cjobid_la_SOURCES = \
        cjobid.c \
        strmd5.c

jobidincludedir = $(includedir)/glite/wmsutils/jobid
jobidinclude_HEADERS = \
	strmd5.h 

AM_CPPFLAGS = -I$(top_srcdir)/src \
	-I$(top_srcdir)/interface \
	$(GLITE_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	-D_GNU_SOURCE

MAINTAINERCLEANFILES = Makefile.in
