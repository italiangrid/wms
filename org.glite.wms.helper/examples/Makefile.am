## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

HELPER = $(top_builddir)/src/libglite_wms_helper.la
HELPER_JOBADAPTER = $(top_builddir)/src/jobadapter/libglite_wms_helper_jobadapter.la

check_PROGRAMS = \
	jobwrapperTest \
	urlTest \
	jobadapterTest

jobadapterTest_SOURCES = jobadapterTest.cpp

jobadapterTest_LDADD   = \
  $(HELPER_JOBADAPTER) \
  $(HELPER) \
  $(GLITE_WMS_COMMON_CONF_LIBS) \
  $(GLITE_WMS_COMMON_LOGGER_LIBS) \
  $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
  $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
  $(GLITE_WMSUTILS_CLASSADS_LIBS) \
  $(GLITE_JDL_LIBS) \
  $(GLOBUS_SSL_THR_LIBS) \
  $(CLASSAD_LIBS) \
  $(BOOST_LIBS)

jobwrapperTest_SOURCES = jobwrapperTest.cpp

jobwrapperTest_LDADD   = \
  $(HELPER_JOBADAPTER) \
  $(HELPER) \
  $(GLITE_WMS_COMMON_CONF_LIBS) \
  $(GLITE_WMS_COMMON_LOGGER_LIBS) \
  $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
  $(GLITE_WMSUTILS_CLASSADS_LIBS) \
  $(GLITE_JDL_LIBS) \
  $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
  $(GLOBUS_SSL_THR_LIBS) \
  $(CLASSAD_LIBS) \
  $(BOOST_LIBS)

urlTest_SOURCES = urlTest.cpp

urlTest_LDADD   =  \
  ../../src/jobadapter/url.o \
  $(BOOST_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/src \
	-I$(top_srcdir)/interface \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) 

EXTRA_DIST = \
	inputclassad \
	inputclassad.new \
  inputclassad.normal \
	inputclassad.od \
	inputclassad.old \
	inputfile1 \
	inputfile2 \
	jobadapter.conf

CLEANFILES = outputclassad \
  localjobadapter.conf 

MAINTAINERCLEANFILES = Makefile.in *.*~

