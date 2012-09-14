## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.egee.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wmsutils_tls_socket_pp.la \
	libglite_wmsutils_tls_gsisocket_pp.la

libglite_wmsutils_tls_socket_pp_la_SOURCES = \
	SocketClient.cpp \
	SocketServer.cpp \
	SocketAgent.cpp

libglite_wmsutils_tls_gsisocket_pp_la_SOURCES = \
	GSISocketAgent.cpp \
	GSISocketClient.cpp \
	GSISocketServer.cpp \
	tokens.cpp

libglite_wmsutils_tls_gsisocket_pp_la_LIBADD = \
	SocketClient.lo \
	SocketServer.lo \
	SocketAgent.lo \
	$(GLOBUS_SSL_UTILS_LIBS) 

socketppincludedir = $(includedir)/glite/wmsutils/tls/socket++
socketppinclude_HEADERS = \
	tokens.h \
	errors.h

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	-DWITH_SOCKET_EXCEPTIONS \
	-DWITH_GLOBUS_TOOLKIT_2_2

MAINTAINERCLEANFILES = Makefile.in
