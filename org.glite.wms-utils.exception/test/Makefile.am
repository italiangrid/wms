## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

EXCEPTION_LIBS = $(top_builddir)/src/libglite_wmsutils_exception.la

TESTS = glite-wmsutils-exception

check_PROGRAMS = $(TESTS)

glite_wmsutils_exception_SOURCES = exception_cu_suite.cpp \
	exception_cu_suite.h \
	exception_cu_main.cpp
	
glite_wmsutils_exception_LDADD =  \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLOBUS_GSS_THR_LIBS) \
	$(EXCEPTION_LIBS) \
	$(CPPUNIT_LIBS) 

AM_CPPFLAGS =  \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test \
	$(GLITE_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	$(CPPUNIT_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~

