## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

GSISOCKET_PP = $(top_builddir)/src/socket++/libglite_wmsutils_tls_gsisocket_pp.la
SOCKET_PP = $(top_builddir)/src/socket++/libglite_wmsutils_socket_pp.la

check_PROGRAMS = glite-wmsutils-socketppServerTest \
        glite-wmsutils-socketppClientTest 

glite_wmsutils_socketppServerTest_SOURCES = socketppServerTest.cpp
glite_wmsutils_socketppServerTest_LDADD = \
	$(GSISOCKET_PP) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLOBUS_GSS_THR_LIBS)

glite_wmsutils_socketppClientTest_SOURCES = socketppClientTest.cpp
glite_wmsutils_socketppClientTest_LDADD = \
	$(GSISOCKET_PP) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLOBUS_GSS_THR_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(GLOBUS_THR_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~

