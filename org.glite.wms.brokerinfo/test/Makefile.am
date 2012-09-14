check_PROGRAMS = glite_wms_brokerinfo_catalog_accesTest


glite_wms_brokerinfo_catalog_accesTest_SOURCES = \
        brokerinfo_catalog_accesTest.cpp


glite_wms_brokerinfo_catalog_accesTest_LDADD = \
	$(top_srcdir)/build/src/.libs/libglite_wms_brokerinfo.a\
	$(GLITE_SERVICE_DISCOVERY_API_C_LIBS)\
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_JDL_LIBS) \
	$(GLITE_WMS_ISM_LIBS) \
	$(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMS_COMMON_UT_II_LIBS)  \
	$(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLOBUS_LDAP_THR_LIBS) \
	$(GLOBUS_SSL_THR_LIBS) \
	$(GLOBUS_GSS_THR_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS)

#$(GLITE_SERVICE_DISCOVERY_API_C_LIBS)


CXXFLAGS = -g

AM_CPPFLAGS = -I$(top_srcdir)/interface \
        -I$(top_srcdir)/src \
        -I$(top_srcidr)/test \
	-I$(GLITE_LOCATION)/include \
        $(GSOAP_CFLAGS) \
        $(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
        $(GLITE_WMS_ISM_CFLAGS) 

MAINTAINERCLEANFILES = Makefile.in *~

