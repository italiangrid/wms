## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

STDCPP = -lstdc++

JOBID = $(top_builddir)/src/jobid/libglite_wmsutils_cjobid.la

check_PROGRAMS = testjobid 

testjobid_SOURCES = testjobid.c
testjobid_LDADD = \
	$(JOBID) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLOBUS_SSL_THR_LIBS) \
	$(STDCPP)

AM_CPPFLAGS = -I$(top_srcdir)/src \
	-I$(top_srcdir)/interface \
        $(GLITE_CFLAGS) \
	$(GLOBUS_THR_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~

