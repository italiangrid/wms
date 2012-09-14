check_PROGRAMS = \
	glite_wms_rls_dli_accessTest\
	glite_wms_rlsTest


glite_wms_rls_dli_accessTest_SOURCES = \
	dli_accessTest.cpp


glite_wms_rls_dli_accessTest_LDADD = \
	$(top_srcdir)/build/src/libglite_wms_replicaservice.la\
	$(top_srcdir)/build/src/DLI/libglite_wms_rls_dli.la\
	$(GLITE_JDL_LIBS)\
	$(GLITE_SERVICE_DISCOVERY_API_C_LIBS)\
        $(GLITE_WMS_COMMON_LOGGER_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMS_COMMON_UT_II_LIBS)  \
        $(GLITE_WMSUTILS_JOBID_LIBS) \
        $(GLITE_WMSUTILS_CJOBID_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
  $(GLITE_LB_THR_CLIENT_LIBS) \
  $(GLITE_LB_THR_CLIENTPP_LIBS) \
  $(GLITE_LB_THR_COMMON_LIBS) \
        $(GLOBUS_LDAP_THR_LIBS) \
        $(GLOBUS_SSL_THR_LIBS) \
        $(GLOBUS_GSS_THR_LIBS) \
        $(CLASSAD_LIBS) \
        $(BOOST_LIBS)

glite_wms_rlsTest_SOURCES = \
	rlsTest.cpp

glite_wms_rlsTest_LDADD = \
	$(top_srcdir)/build/src/libglite_wms_replicaservice.la\
	$(GLITE_JDL_LIBS)\
	$(GLITE_SERVICE_DISCOVERY_API_C_LIBS)\
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMS_COMMON_UT_II_LIBS)  \
	$(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
  $(GLITE_LB_THR_CLIENT_LIBS) \
  $(GLITE_LB_THR_CLIENTPP_LIBS) \
  $(GLITE_LB_THR_COMMON_LIBS) \
	$(GLOBUS_LDAP_THR_LIBS) \
	$(GLOBUS_SSL_THR_LIBS) \
	$(GLOBUS_GSS_THR_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS)


CXXFLAGS = -g

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/DLI \
	-I$(GLITE_LOCATION)/include \
	$(GSOAP_CFLAGS) \
	$(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(GLOBUS_THR_CFLAGS)\
	$(GLITE_WMS_COMMON_SYSLOG_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~
