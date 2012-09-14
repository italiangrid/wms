## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES    = libglite_wmsutils_classads.la

libglite_wmsutils_classads_la_SOURCES = \
        classad_utils.cpp

# Can do the following only when libclassad.so exists
#libglite_wmsutils_classads_la_LIBADD = $(CLASSAD_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
        -I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(CLASSAD_CPPFLAGS)

MAINTAINERCLEANFILES = Makefile.in *.*~
