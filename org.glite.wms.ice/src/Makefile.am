#
#       Copyright (c) 2004 on behalf of the EU EGEE Project:
#       The European Organization for Nuclear Research (CERN),
#       Istituto Nazionale di Fisica Nucleare (INFN), Italy
#       Datamat Spa, Italy
#       Centre National de la Recherche Scientifique (CNRS), France
#       CS Systeme d'Information (CSSI), France
#       Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
#       Universiteit van Amsterdam (UvA), Netherlands
#       University of Helsinki (UH.HIP), Finland
#       University of Bergen (UiB), Norway
#       Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom
#

SUBDIRS = iceCommands ice iceThreads iceUtils iceDb iceDb/sqlite

bin_PROGRAMS =  putFL \
		putFL_cancel \
		putFL_reschedule \
		glite-wms-ice \
		glite-wms-ice-safe \
		queryDb queryStats \
		glite-wms-ice-proxy-renew \
		glite-wms-ice-rm \
		glite-wms-ice-db-rm

glite_wms_ice_db_rm_SOURCES = glite-wms-ice-db-rm.cpp

glite_wms_ice_rm_SOURCES = glite-wms-ice-rm.cpp

glite_wms_ice_proxy_renew_SOURCES = glite-wms-ice-proxy-renew.cpp

queryDb_SOURCES = queryDb.cpp

queryStats_SOURCES = queryStats.cpp

putFL_SOURCES = putFL.cpp

putFL_cancel_SOURCES = putFL_cancel.cpp

putFL_reschedule_SOURCES = putFL_reschedule.cpp

glite_wms_ice_SOURCES = glite-wms-ice.cpp

glite_wms_ice_safe_SOURCES = glite-wms-ice-safe.cpp

queryDb_LDADD = $(EXPORT_DYNAMIC_LDFLAGS) \
		$(GRIDSITE_OPENSSL_LIBS) \
		$(XML2_LIBS) \
		$(GLITE_CREAM_LIBS) \
		$(GSOAP_PLUGIN_LIBS) \
		$(GSOAP_LIBS) \
		$(LOG4CPP_LIBS) \
		../src/iceUtils/libglite_wms_iceUtils.la \
		../src/iceThreads/libglite_wms_iceThreads.la \
		../src/iceCommands/libglite_wms_iceCommands.la \
		../src/ice/libglite_wms_ice.la \
		../src/iceDb/libglite_wms_iceDb.la \
		$(GLITE_WMSUTILS_CLASSADS_LIBS) \
		$(CLASSAD_LIBS) \
		$(ARES_LIBS) \
		$(GLITE_JOBID_LIBS) \
		$(GLITE_WMS_COMMON_CONF_LIBS) \
		$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
		$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
		$(GLITE_LB_CLIENT_LIBS) \
		$(GLITE_LB_CLIENTPP_LIBS) \
		$(GLITE_LB_COMMON_LIBS) \
		$(RENEWAL_LIBS) \
                $(GLITE_WMS_PURGER_LIBS) \
                $(GLITE_WMS_COMMON_LOGGER_LIBS) \
		$(BOOST_FS_LIBS) \
		$(BOOST_THREAD_LIBS) \
		$(BOOST_REGEX_LIBS) \
		$(BOOST_PO_LIBS) \
		-lboost_date_time \
		-lboost_serialization \
		$(VOMS_LIBS) \
		iceDb/sqlite/libsqlite3.a
		
queryStats_LDADD = $(EXPORT_DYNAMIC_LDFLAGS) \
		$(GRIDSITE_OPENSSL_LIBS) \
		$(XML2_LIBS) \
                $(GLITE_CREAM_LIBS) \
		$(GSOAP_PLUGIN_LIBS) \
		$(GSOAP_LIBS) \
		$(LOG4CPP_LIBS) \
		../src/iceUtils/libglite_wms_iceUtils.la \
		../src/iceThreads/libglite_wms_iceThreads.la \
		../src/iceCommands/libglite_wms_iceCommands.la \
		../src/ice/libglite_wms_ice.la \
		../src/iceDb/libglite_wms_iceDb.la \
		$(GLITE_WMSUTILS_CLASSADS_LIBS) \
		$(CLASSAD_LIBS) \
                $(ARES_LIBS) \
		$(GLITE_JOBID_LIBS) \
		$(GLITE_WMS_COMMON_CONF_LIBS) \
		$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
		$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
		$(GLITE_LB_CLIENT_LIBS) \
		$(GLITE_LB_CLIENTPP_LIBS) \
		$(GLITE_LB_COMMON_LIBS) \
		$(RENEWAL_LIBS) \
                $(GLITE_WMS_PURGER_LIBS) \
                $(GLITE_WMS_COMMON_LOGGER_LIBS) \
		$(BOOST_FS_LIBS) \
		$(BOOST_THREAD_LIBS) \
		$(BOOST_REGEX_LIBS) \
		$(BOOST_PO_LIBS) \
		-lboost_date_time \
		-lboost_serialization \
		$(VOMS_LIBS) \
		iceDb/sqlite/libsqlite3.a
		
putFL_LDADD =  $(EXPORT_DYNAMIC_LDFLAGS) \
		$(GRIDSITE_OPENSSL_LIBS) \
		$(XML2_LIBS) \
		$(GLITE_CREAM_LIBS) \
		$(GSOAP_PLUGIN_LIBS) \
 		$(LOG4CPP_LIBS) \
		../src/iceUtils/libglite_wms_iceUtils.la \
		../src/iceThreads/libglite_wms_iceThreads.la \
		../src/iceCommands/libglite_wms_iceCommands.la \
		../src/ice/libglite_wms_ice.la \
		../src/iceDb/libglite_wms_iceDb.la \
		$(GLITE_WMSUTILS_CLASSADS_LIBS) \
		$(CLASSAD_LIBS) \
                $(ARES_LIBS) \
		$(GLITE_JOBID_LIBS) \
		$(GLITE_WMS_COMMON_CONF_LIBS) \
		$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
		$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
		$(GLITE_LB_CLIENT_LIBS) \
		$(GLITE_LB_CLIENTPP_LIBS) \
		$(GLITE_LB_COMMON_LIBS) \
		$(RENEWAL_LIBS) \
                $(GLITE_WMS_PURGER_LIBS) \
                $(GLITE_WMS_COMMON_LOGGER_LIBS) \
		$(BOOST_FS_LIBS) \
		$(BOOST_THREAD_LIBS) \
		$(BOOST_REGEX_LIBS) \
		$(BOOST_PO_LIBS) \
		-lboost_date_time \
		-lboost_serialization \
		iceDb/sqlite/libsqlite3.a

putFL_reschedule_LDADD = $(EXPORT_DYNAMIC_LDFLAGS) \
			$(GRIDSITE_OPENSSL_LIBS) \
			$(XML2_LIBS) \
	                $(GLITE_CREAM_LIBS) \
			$(GSOAP_PLUGIN_LIBS) \
			$(GSOAP_LIBS) \
			$(LOG4CPP_LIBS) \
			../src/iceUtils/libglite_wms_iceUtils.la \
			../src/iceThreads/libglite_wms_iceThreads.la \
			../src/iceCommands/libglite_wms_iceCommands.la \
			../src/ice/libglite_wms_ice.la \
			../src/iceDb/libglite_wms_iceDb.la \
			$(GLITE_WMSUTILS_CLASSADS_LIBS) \
			$(CLASSAD_LIBS) \
			$(ARES_LIBS) \
			$(GLITE_JOBID_LIBS) \
			$(GLITE_WMS_COMMON_CONF_LIBS) \
			$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
			$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
			$(GLITE_LB_CLIENT_LIBS) \
			$(GLITE_LB_CLIENTPP_LIBS) \
			$(GLITE_LB_COMMON_LIBS) \
			$(RENEWAL_LIBS) \
			$(GLITE_WMS_PURGER_LIBS) \
			$(GLITE_WMS_COMMON_LOGGER_LIBS) \
			$(BOOST_FS_LIBS) \
			$(BOOST_THREAD_LIBS) \
			$(BOOST_REGEX_LIBS) \
			$(BOOST_PO_LIBS) \
			-lboost_date_time \
			-lboost_serialization \
			$(VOMS_LIBS) \
			iceDb/sqlite/libsqlite3.a

putFL_cancel_LDADD = $(LOG4CPP_LIBS) $(EXPORT_DYNAMIC_LDFLAGS) \
		$(BOOST_FS_LIBS) \
		$(BOOST_THREAD_LIBS) \
		$(BOOST_REGEX_LIBS) \
		$(GLITE_WMSUTILS_CLASSADS_LIBS) \
		$(CLASSAD_LIBS) \
                $(ARES_LIBS) \
		$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
		$(GLITE_JOBID_LIBS) \
		$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
		$(GLITE_LB_CLIENT_LIBS) \
		$(GLITE_LB_CLIENTPP_LIBS) \
		$(GLITE_LB_COMMON_LIBS) \
                $(GLITE_WMS_COMMON_LOGGER_LIBS) \
		$(RENEWAL_LIBS) \
		-lboost_date_time \
		-lboost_serialization \
		iceDb/sqlite/libsqlite3.a

glite_wms_ice_safe_LDADD = $(BOOST_PO_LIBS) $(EXPORT_DYNAMIC_LDFLAGS) \
		$(GRIDSITE_OPENSSL_LIBS) \
		$(XML2_LIBS) \
                $(GLITE_CREAM_LIBS) \
		$(GSOAP_PLUGIN_LIBS) \
		$(GSOAP_LIBS) \
		$(LOG4CPP_LIBS) \
		../src/iceUtils/libglite_wms_iceUtils.la \
		../src/iceThreads/libglite_wms_iceThreads.la \
		../src/iceCommands/libglite_wms_iceCommands.la \
		../src/ice/libglite_wms_ice.la \
		../src/iceDb/libglite_wms_iceDb.la \
		$(GLITE_WMSUTILS_CLASSADS_LIBS) \
		$(CLASSAD_LIBS) \
                $(ARES_LIBS) \
		$(GLITE_JOBID_LIBS) \
		$(GLITE_WMS_COMMON_CONF_LIBS) \
		$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
		$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
		$(GLITE_LB_CLIENT_LIBS) \
		$(GLITE_LB_CLIENTPP_LIBS) \
		$(GLITE_LB_COMMON_LIBS) \
		$(RENEWAL_LIBS) \
                $(GLITE_WMS_PURGER_LIBS) \
                $(GLITE_WMS_COMMON_LOGGER_LIBS) \
		$(BOOST_FS_LIBS) \
		$(BOOST_THREAD_LIBS) \
		$(BOOST_REGEX_LIBS) \
		$(BOOST_PO_LIBS) \
		-lboost_date_time \
		-lboost_serialization \
		$(VOMS_LIBS) \
		iceDb/sqlite/libsqlite3.a

glite_wms_ice_rm_LDADD = $(EXPORT_DYNAMIC_LDFLAGS) \
		$(GRIDSITE_OPENSSL_LIBS) \
		$(XML2_LIBS) \
                $(GLITE_CREAM_LIBS) \
		$(GSOAP_PLUGIN_LIBS) \
		$(GSOAP_LIBS) \
		$(LOG4CPP_LIBS) \
		../src/iceUtils/libglite_wms_iceUtils.la \
		../src/iceThreads/libglite_wms_iceThreads.la \
		../src/iceCommands/libglite_wms_iceCommands.la \
		../src/ice/libglite_wms_ice.la \
		../src/iceDb/libglite_wms_iceDb.la \
		$(GLITE_WMSUTILS_CLASSADS_LIBS) \
		$(CLASSAD_LIBS) \
                $(ARES_LIBS) \
		$(GLITE_JOBID_LIBS) \
		$(GLITE_WMS_COMMON_CONF_LIBS) \
		$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
		$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
		$(GLITE_LB_CLIENT_LIBS) \
		$(GLITE_LB_CLIENTPP_LIBS) \
		$(GLITE_LB_COMMON_LIBS) \
		$(RENEWAL_LIBS) \
                $(GLITE_WMS_PURGER_LIBS) \
                $(GLITE_WMS_COMMON_LOGGER_LIBS) \
		$(BOOST_FS_LIBS) \
		$(BOOST_THREAD_LIBS) \
		$(BOOST_REGEX_LIBS) \
		$(BOOST_PO_LIBS) \
		-lboost_date_time \
		-lboost_serialization \
		$(VOMS_LIBS) \
		iceDb/sqlite/libsqlite3.a
		
glite_wms_ice_db_rm_LDADD = $(EXPORT_DYNAMIC_LDFLAGS) \
		$(GRIDSITE_OPENSSL_LIBS) \
		$(XML2_LIBS) \
                $(GLITE_CREAM_LIBS) \
		$(GSOAP_PLUGIN_LIBS) \
		$(GSOAP_LIBS) \
		$(LOG4CPP_LIBS) \
		../src/iceUtils/libglite_wms_iceUtils.la \
		../src/iceThreads/libglite_wms_iceThreads.la \
		../src/iceCommands/libglite_wms_iceCommands.la \
		../src/ice/libglite_wms_ice.la \
		../src/iceDb/libglite_wms_iceDb.la \
		$(GLITE_WMSUTILS_CLASSADS_LIBS) \
		$(CLASSAD_LIBS) \
                $(ARES_LIBS) \
		$(GLITE_JOBID_LIBS) \
		$(GLITE_WMS_COMMON_CONF_LIBS) \
		$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
		$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
		$(GLITE_LB_CLIENT_LIBS) \
		$(GLITE_LB_CLIENTPP_LIBS) \
		$(GLITE_LB_COMMON_LIBS) \
		$(RENEWAL_LIBS) \
                $(GLITE_WMS_PURGER_LIBS) \
                $(GLITE_WMS_COMMON_LOGGER_LIBS) \
		$(BOOST_FS_LIBS) \
		$(BOOST_THREAD_LIBS) \
		$(BOOST_REGEX_LIBS) \
		$(BOOST_PO_LIBS) \
		-lboost_date_time \
		-lboost_serialization \
		$(VOMS_LIBS) \
		iceDb/sqlite/libsqlite3.a

glite_wms_ice_LDADD = $(EXPORT_DYNAMIC_LDFLAGS) \
		$(GRIDSITE_OPENSSL_LIBS) \
		$(XML2_LIBS) \
                $(GLITE_CREAM_LIBS) \
		$(GSOAP_PLUGIN_LIBS) \
		$(GSOAP_LIBS) \
		$(LOG4CPP_LIBS) \
		../src/iceUtils/libglite_wms_iceUtils.la \
		../src/iceThreads/libglite_wms_iceThreads.la \
		../src/iceCommands/libglite_wms_iceCommands.la \
		../src/ice/libglite_wms_ice.la \
		../src/iceDb/libglite_wms_iceDb.la \
		$(GLITE_WMSUTILS_CLASSADS_LIBS) \
		$(CLASSAD_LIBS) \
                $(ARES_LIBS) \
		$(GLITE_JOBID_LIBS) \
		$(GLITE_WMS_COMMON_CONF_LIBS) \
		$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
		$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
		$(GLITE_LB_CLIENT_LIBS) \
		$(GLITE_LB_CLIENTPP_LIBS) \
		$(GLITE_LB_COMMON_LIBS) \
		$(RENEWAL_LIBS) \
                $(GLITE_WMS_PURGER_LIBS) \
                $(GLITE_WMS_COMMON_LOGGER_LIBS) \
		$(BOOST_FS_LIBS) \
		$(BOOST_THREAD_LIBS) \
		$(BOOST_REGEX_LIBS) \
		$(BOOST_PO_LIBS) \
		-lboost_date_time \
		-lboost_serialization \
		$(VOMS_LIBS) \
		iceDb/sqlite/libsqlite3.a

glite_wms_ice_proxy_renew_LDADD = $(EXPORT_DYNAMIC_LDFLAGS) \
		$(GLOBUS_GSI_CRED_LIBS) \
		$(GLOBUS_GSI_PROXY_LIBS) \
		$(GLOBUS_GSI_UTILS_LIBS) \
		$(MYPROXY_LIBS) \
		$(BOOST_FS_LIBS) \
		$(RENEWAL_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/src \
		$(GLITE_CREAM_CFLAGS) \
		$(VOMS_CFLAGS) \
		$(GSOAP_CFLAGS) \
		$(CLASSAD_CFLAGS) \
		$(BOOST_CFLAGS) \
		-UHAVE_CONFIG_H \
		$(LOG4CPP_CFLAGS) \
		$(GLOBUS_GSI_CRED_CFLAGS) \
		$(GLOBUS_GSI_PROXY_CFLAGS) \
		$(GLOBUS_GSI_UTILS_CFLAGS) \
		$(MYPROXY_CFLAGS) \
		-pipe \
		-DICE_VERSIONID=\""`date`"\"

MAINTAINERCLEANFILES = Makefile.in
