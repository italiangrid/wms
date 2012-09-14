## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wmsutils_tls_ssl_helpers.la \
	libglite_wmsutils_tls_ssl_pthr_helpers.la

libglite_wmsutils_tls_ssl_helpers_la_SOURCES = \
	ssl_inits.c

libglite_wmsutils_tls_ssl_pthr_helpers_la_SOURCES = \
	ssl_pthreads.c

ssl_helpersincludedir = $(includedir)/glite/wmsutils/tls/ssl_helpers
ssl_helpersinclude_HEADERS = \
	ssl_inits.h \
	ssl_pthreads.h

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GLOBUS_THR_CFLAGS) \
	$(GLITE_CFLAGS) \
	-D_GNU_SOURCE

MAINTAINERCLEANFILES = Makefile.in
