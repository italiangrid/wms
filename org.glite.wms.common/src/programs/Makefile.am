## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

UTIL = $(top_builddir)/src/utilities/libglite_wms_util.la
CONFIG = $(top_builddir)/src/configuration/libglite_wms_conf.la
LOGGER = $(top_builddir)/src/logger/libglite_wms_logger.la

bin_PROGRAMS = glite-wms-get-configuration

glite_wms_get_configuration_SOURCES = get_configuration.cpp
glite_wms_get_configuration_LDADD   = \
	$(CONFIG) \
	$(UTIL) \
  $(LOGGER) \
  $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
  $(GLITE_WMSUTILS_JOBID_LIBS) \
  $(GLITE_WMSUTILS_CJOBID_LIBS) \
  $(GLITE_WMSUTILS_CLASSADS_LIBS) \
  $(GLITE_LB_THR_CLIENT_LIBS) \
  $(GLITE_LB_THR_CLIENTPP_LIBS) \
  $(GLITE_LB_THR_COMMON_LIBS) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) 

MAINTAINERCLEANFILES = Makefile.in *~

