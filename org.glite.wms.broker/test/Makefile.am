check_PROGRAMS = glite_wms_broker_test glite_wms_helper_test

glite_wms_broker_test_SOURCES = \
        broker_test.cpp 

glite_wms_helper_test_SOURCES = \
	helper_test.cpp


glite_wms_broker_test_LDFLAGS = -Xlinker --export-dynamic

glite_wms_helper_test_LDFLAGS = -Xlinker --export-dynamic

glite_wms_broker_test_LDADD = \
	$(top_srcdir)/../org.glite.wms.manager/build/src/server/signal_handling.o\
	$(top_srcdir)/../org.glite.wms.manager/build/src/daemons/ism_utils.o\
	$(top_srcdir)/../org.glite.wms.manager/build/src/server/dynamic_library.o\
        $(top_srcdir)/build/src/libglite_wms_broker.la \
        $(GLITE_SERVICE_DISCOVERY_API_C_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_JDL_LIBS) \
        $(GLITE_WMS_RLS_LIBS) \
	$(GLITE_WMS_ISM_LIBS) \
	$(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMS_COMMON_UT_II_LIBS)  \
	$(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLOBUS_LDAP_THR_LIBS) \
	$(GLOBUS_SSL_THR_LIBS) \
	$(GLOBUS_GSS_THR_LIBS) \
	$(CLASSAD_DL_LIBS) \
        $(top_srcdir)/../org.glite.wms.classad_plugin/build/src/libglite_wms_classad_plugin_loader.la \
	$(BOOST_LIBS)

glite_wms_helper_test_LDADD = \
	$(top_srcdir)/../org.glite.wms.classad_plugin/build/src/libglite_wms_classad_plugin_loader.la \
        $(top_srcdir)/build/src/libglite_wms_broker.la \
	$(GLITE_LOCATION)/lib/libglite_wms_helper.la \
        $(GLITE_SERVICE_DISCOVERY_API_C_LIBS) \
        $(GLITE_WMS_COMMON_LOGGER_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_JDL_LIBS) \
        $(GLITE_WMS_RLS_LIBS) \
        $(GLITE_WMS_ISM_LIBS) \
        $(GLITE_WMS_ISM_II_PURCHASER_LIBS)\
        $(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMS_COMMON_UT_II_LIBS)  \
        $(GLITE_WMSUTILS_JOBID_LIBS) \
        $(GLITE_WMSUTILS_CJOBID_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLOBUS_LDAP_THR_LIBS) \
        $(GLOBUS_SSL_THR_LIBS) \
        $(GLOBUS_GSS_THR_LIBS) \
	$(CLASSAD_DL_LIBS) \
        $(BOOST_LIBS)

CXXFLAGS = -g

AM_CPPFLAGS = \
        -I$(top_srcdir)/src \
        -I$(top_srcidr)/test \
	-I$(GLITE_LOCATION)/include \
	-I$(top_srcdir)/../org.glite.wms.manager/src/server \
	-I$(top_srcdir)/../org.glite.wms.manager/src/daemons \
        $(GSOAP_CFLAGS) \
        $(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
        $(GLITE_WMS_ISM_CFLAGS) \
	-DGLITE_WMS_DONT_HAVE_GPBOX \
	$(GLITE_WMS_COMMON_SYSLOG_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~

