## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

GSISOCKET_PP = $(top_builddir)/src/socket++/libglite_wmsutils_tls_gsisocket_pp.la
SOCKET_PP = $(top_builddir)/src/socket++/libglite_wmsutils_socket_pp.la

TESTS = gsiclient

check_PROGRAMS = $(TESTS)

gsiclient_SOURCES = GSIServerClient.cpp
gsiclient_LDADD = \
	$(GSISOCKET_PP) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLOBUS_GSS_THR_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src/socket++ \
	-I$(top_srcdir)/test \
	$(GLITE_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	$(CPPUNIT_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~

