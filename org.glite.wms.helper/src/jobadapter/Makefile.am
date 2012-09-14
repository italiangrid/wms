## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wms_helper_jobadapter.la

noinst_HEADERS = \
	exceptions.h \
  url.h \
  JobWrapper.h \
	Helper.h

indir = $(includedir)/glite/wms/helper/jobadapter
in_HEADERS = JobAdapter.h

libglite_wms_helper_jobadapter_la_SOURCES = \
  exceptions.cpp \
  JobWrapper.cpp \
  url.cpp \
  JobAdapter.cpp \
  Helper.cpp

libglite_wms_helper_jobadapter_la_LIBADD = \
        $(GLITE_JDL_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(GLITE_JDL_CFLAGS) \
	$(GLITE_WMS_COMMON_CONF_CFLAGS) \
	$(GLITE_WMS_COMMON_UT_UTIL_CFLAGS) \
	$(GLITE_WMSUTILS_CLASSADS_CFLAGS) \
	$(GLITE_WMSUTILS_EXCEPTION_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *.*~
