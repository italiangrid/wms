bin_PROGRAMS = glite_wms_helper_test

check_PROGRAMS = \
  ldap_search \
  glite_wms_brokerinfo_catalog_accesTest \
  ldap_search_async \
  ii_purchaserTest \
  ii_g2_purchaserTest

glite_wms_brokerinfo_catalog_accesTest_SOURCES = \
        brokerinfo_catalog_accesTest.cpp

glite_wms_brokerinfo_catalog_accesTest_LDADD = \
        $(GLITE_SERVICE_DISCOVERY_API_C_LIBS)\
        $(GLITE_WMS_COMMON_LOGGER_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_JDL_LIBS) \
        -L$(top_srcdir)/build/src/.libs -lglite_wms_dli -lglite_wms_brokerinfo \
        -L$(top_srcdir)/stage/lib64 -lglite_wms_broker -lglite_wms_matchmaking -lglite-sd-c\
        $(GLITE_WMS_ISM_LIBS) \
        $(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMS_COMMON_UT_II_LIBS)  \
        $(GLITE_JOBID_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMS_COMMON_UT_II_LIBS)  \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
        -lldap -llber \
        $(GLOBUS_SSL_THR_LIBS) \
        $(GLOBUS_GSS_THR_LIBS) \
        $(CLASSAD_LIBS) \
        $(BOOST_LIBS)

ldap_search_SOURCES = \
	ldap_searchTest.cpp

ldap_search_LDADD = \
        -lldap \
        $(BOOST_LIBS)

ldap_search_async_SOURCES = \
	ldap_searchTestasync.cpp

ldap_search_async_LDADD = \
        -lldap \
        $(BOOST_LIBS)

ii_g2_purchaserTest_SOURCES = \
        ism-ii-g2-purchaserTest.cpp

ii_g2_purchaserTest_LDADD = \
        $(top_srcdir)/build/src/libglite_wms_ism.la \
        $(top_srcdir)/build/src/purchaser/libglite_wms_ism_ii_g2_purchaser.la \
        -lldap \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_JDL_LIBS) \
        $(GLITE_WMS_COMMON_LOGGER_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
        $(CLASSAD_LIBS) \
        $(BOOST_LIBS)

ii_g2_purchaserTest_CPPFLAGS = \
	-I$(top_srcdir)/interface \
	$(GLITE_WMS_COMMON_LOGGER_CFLAGS) \
	$(CLASSAD_CFLAGS) \
        $(BOOST_CFLAGS)

ii_purchaserTest_SOURCES = \
        ism-ii-purchaserTest.cpp

ii_purchaserTest_LDADD = \
        $(top_srcdir)/src/libglite_wms_ism.la \
        $(top_srcdir)/src/purchaser/libglite_wms_ism_ii_purchaser.la \
        -lldap \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_JDL_LIBS) \
        $(GLITE_WMS_COMMON_LOGGER_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
        $(CLASSAD_LIBS) \
        $(BOOST_LIBS)

glite_wms_helper_test_SOURCES = \
	helper_test.cpp

glite_wms_helper_test_LDFLAGS = -Xlinker --export-dynamic

glite_wms_helper_test_LDADD = \
        $(top_srcdir)/build/src/libglite_wms_helper.la \
	$(GLITE_WMS_CLASSAD_PLUGIN_LIBS) \
        $(GLITE_WMS_COMMON_LOGGER_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_JOBID_LIBS) \
        $(GLITE_JDL_LIBS) -lglite_jobid \
	$(GLITE_WMS_ISM_LIBS) \
        $(GLITE_WMS_ISM_II_PURCHASER_LIBS)\
        $(GLITE_WMS_ISM_II_G2_PURCHASER_LIBS)\
        $(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMS_COMMON_UT_II_LIBS)  \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) -lldap -llber \
        $(GLOBUS_LDAP_THR_LIBS) \
        $(GLOBUS_SSL_THR_LIBS) \
        $(GLOBUS_GSS_THR_LIBS) \
	$(CLASSAD_LIBS) \
        $(BOOST_LIBS)

CXXFLAGS = -g -O0

AM_CPPFLAGS = \
        -I$(top_srcdir)/src \
	-I$(top_srcdir)/src/broker \
        -I$(top_srcdir)/test \
	-I$(top_srcdir)/interface \
	-I$(GLITE_LOCATION)/include \
        $(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
        $(GLITE_WMS_ISM_CFLAGS) 

MAINTAINERCLEANFILES = Makefile.in *~
