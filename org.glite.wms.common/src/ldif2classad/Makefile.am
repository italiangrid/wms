## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wms_LDIF2ClassAd.la

libglite_wms_LDIF2ClassAd_la_SOURCES = \
	LDAPQuery.cpp \
	LDAPSynchConnection.cpp \
	FileStreamConnection.cpp \
	LDAPFilterParser.cpp\
	LDIFObject.cpp \
	LDAPForwardIterator.cpp

ldif2classadincludedir = $(includedir)/glite/wms/common/ldif2classad
ldif2classadinclude_HEADERS = \
	LDAPFilterParser.h \
	LDAPForwardIterator.h 

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	$(BOOST_CFLAGS) \
        $(LDAP_CPPFLAGS) \
	-DWITH_LDAP_EXCEPTIONS \
	-DDONT_QUOTE_TRUE_FALSE

MAINTAINERCLEANFILES = Makefile.in
