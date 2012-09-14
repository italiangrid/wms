## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

SUBDIRS = oldgaa

lib_LTLIBRARIES = libglobus_ssl_utils.la

libglobus_ssl_utils_la_SOURCES = \
	scutils.c \
	sslutils.c
libglobus_ssl_utils_la_LIBADD = ${top_builddir}/src/oldgaa/libglobus_oldgaa.la

globus_ssl_utilsincludedir = $(includedir)/glite/wmsutils/thirdparty/globus_ssl_utils
globus_ssl_utilsinclude_HEADERS = \
	scutils.h \
	sslutils.h

AM_CPPFLAGS = -I${srcdir}/oldgaa \
	$(GLOBUS_THR_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in
