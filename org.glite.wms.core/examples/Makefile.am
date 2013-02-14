## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

bin_PROGRAMS = \
	jobwrapperTest \
	urlTest #\
	#jobadapterTest glite_wms_ism_timeutc \
	#glite_wms_ism_dump_file \
	#glite_wms_ism_ii_purchaser

glite_wms_ism_timeutc_SOURCES = \
	timeUTC.cpp

glite_wms_ism_timeutc_LDADD = \
        $(BOOST_LIBS)

glite_wms_ism_dump_file_SOURCES = \
        ismdumpfile.cpp

glite_wms_ism_dump_file_LDADD = \
        $(GLOBUS_SSL_THR_LIBS) \
        $(CLASSAD_LIBS) \
        $(BOOST_LIBS)

glite_wms_ism_ii_purchaser_SOURCES = \
	ism-ii-purchaser.cpp

glite_wms_ism_ii_purchaser_LDADD = \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMS_COMMON_UT_II_LIBS) \
	$(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(top_builddir)/src/ism/purchaser/libglite_wms_ism_ii_purchaser.la \
	$(top_builddir)/src/ism/purchaser/libglite_wms_ism_file_purchaser.la \
        $(top_builddir)/src/ism/libglite_wms_ism.la \
	$(GLOBUS_LDAP_THR_LIBS) \
        $(GLOBUS_SSL_THR_LIBS) \
        $(CLASSAD_LIBS) \
        $(BOOST_LIBS)

jobadapterTest_SOURCES = jobadapterTest.cpp

jobadapterTest_LDADD   = \
  $(top_builddir)/src/helper/.libs/libglite_wms_helper.so \
  $(top_builddir)/src/helper/jobadapter/.libs/libglite_wms_helper_jobadapter.so \
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
  $(top_builddir)/src/helper/.libs/libglite_wms_helper.so \
  $(top_builddir)/src/helper/jobadapter/.libs/libglite_wms_helper_jobadapter.so \
        $(GLITE_LB_CLIENT_LIBS) \
        $(GLITE_LB_CLIENTPP_LIBS) \
        $(GLITE_LB_COMMON_LIBS) \
  $(GLITE_WMS_COMMON_CONF_LIBS) -lglite_jobid \
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
  $(top_builddir)/src/helper/jobadapter/url.o \
  $(BOOST_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/src \
	-I$(top_srcdir)/interface \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) 

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

