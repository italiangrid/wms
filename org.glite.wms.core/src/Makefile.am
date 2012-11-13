SUBDIRS = ism helper

bin_PROGRAMS = glite-wms-workload_manager glite-wms-query-job-state-transitions

noinst_HEADERS = dispatcher_utils.h \
		signal_handling.h \
		listmatch.h \
		events.h \
		wm_real.h

glite_wms_query_job_state_transitions_SOURCES = glite-wms-query-job-state-transitions.cpp
glite_wms_query_job_state_transitions_LDADD =	\
        $(BOOST_LIBS) \
        $(CLASSAD_LIBS) \
        $(ARES_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_LB_CLIENT_LIBS) \
        $(GLITE_LB_CLIENTPP_LIBS) \
        $(GLITE_LB_COMMON_LIBS)

glite_wms_workload_manager_SOURCES = main.cpp ism_utils.cpp bulkmm_utils.h bulkmm_utils.cpp listmatch.h listmatch.cpp plan.h plan.cpp submit_request.h submit_request.cpp cancel_request.h cancel_request.cpp match_request.h match_request.cpp events.h events.cpp dispatcher_utils.h dispatcher_utils.cpp wm_real.h wm_real.cpp signal_handling.h signal_handling.cpp match_utils.h match_utils.cpp recovery.h recovery.cpp lb_utils.h lb_utils.cpp dynamic_library.h dynamic_library.cpp submission_utils.h submission_utils.cpp replanner.cpp

glite_wms_workload_manager_LDFLAGS = -Xlinker --export-dynamic
glite_wms_workload_manager_LDADD   = \
        $(BOOST_LIBS) \
        $(EXPAT_LIBS) \
        $(CLASSAD_LIBS) \
	$(ARES_LIBS) \
        $(GLITE_WMS_PURGER_LIBS) \
        $(GLITE_JDL_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMS_COMMON_LOGGER_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
        $(GLITE_LB_CLIENT_LIBS) \
        $(GLITE_LB_CLIENTPP_LIBS) \
        $(GLITE_LB_COMMON_LIBS) \
        $(RENEWAL_LIBS) \
        $(LDAP_LIBS) \
        $(top_builddir)/src/ism/purchaser/libglite_wms_ism_ii_purchaser.la \
        $(top_builddir)/src/ism/libglite_wms_ism.la \
        $(top_builddir)/src/helper/jobadapter/libglite_wms_helper_jobadapter.la \
        $(top_builddir)/src/helper/broker/libglite_wms_helper_broker_ism.la \
        $(top_builddir)/src/helper/libglite_wms_helper.la

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/interface \
		$(GLITE_WMS_COMMON_CONF_CFLAGS) \
		$(GLITE_WMS_COMMON_LOGGER_CFLAGS) \
		$(GLITE_WMS_COMMON_UT_UTIL_CFLAGS) \
		$(GLITE_WMS_PURGER_CFLAGS) \
		$(GLITE_JDL_CFLAGS) \
		$(CLASSAD_CFLAGS) \
		$(BOOST_CFLAGS) 

install-exec-hook:
	cp $(top_srcdir)/src/glite-wms-stats.py $(DESTDIR)/$(bindir)/glite-wms-stats

MAINTAINERCLEANFILES = Makefile.in *~ *.*~
