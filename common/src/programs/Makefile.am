## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

UTIL = $(top_builddir)/src/utilities/libglite_wms_util.la
CONFIG = $(top_builddir)/src/configuration/libglite_wms_conf.la

bin_PROGRAMS = glite-wms-get-configuration

glite_wms_get_configuration_SOURCES = get_configuration.cpp
glite_wms_get_configuration_LDADD   = \
	$(CONFIG) \
	$(UTIL) \
        $(GLITE_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) 

MAINTAINERCLEANFILES = Makefile.in *~

