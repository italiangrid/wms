## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

bin_PROGRAMS = glite-wms-manager_ns_daemon

noinst_HEADERS = \
	NetworkServerImpl.h \
	NetworkServerReal.h \
	NS2WMProxy.h \
	exceptions.h \
	logging_fn.h

glite_wms_manager_ns_daemon_SOURCES = \
        NetworkServer.cpp \
	NetworkServerImpl.cpp \
	NetworkServerReal.cpp \
	logging_fn.cpp \
	NS2WMProxy.cpp \
	signal_handling.cpp \
	main.cpp

glite_wms_manager_ns_daemon_LDFLAGS = -Xlinker --export-dynamic

glite_wms_manager_ns_daemon_LDADD = \
	$(GLITE_WMS_NS_COMMON_SERVER_COMMANDS_LIBS) \
	$(GLITE_WMS_NS_COMMON_GLITE_PIPE_LIBS) \
	$(GLITE_WMS_PURGER) \
	$(GLITE_LOCATION)/lib/libglite_wms_quota.la \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMSUTILS_JOBID_LIBS) \
        $(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_JDL_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMSUTILS_TLS_GSI_SOCKET_LIBS) \
        $(GLITE_WMS_COMMON_PROCESS_LIBS) \
        $(GLITE_LB_THR_CLIENT_LIBS) \
	$(GLITE_LB_THR_CLIENTPP_LIBS) \
        $(GLITE_LB_THR_COMMON_LIBS) \
	$(GLITE_SECURITY_RENEWAL_THR_LIBS) \
        $(GLITE_WMS_COMMON_UTIL_LIBS) \
	$(GLITE_WMS_COMMON_UT_II_LIBS) \
        $(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
	$(GLITE_WMS_OPTIONAL_JP_LIBS) \
	$(GLOBUS_SSL_THR_LIBS) \
        $(GLOBUS_COMMON_THR_LIBS) \
        $(GLOBUS_GSS_THR_LIBS) \
        $(GLOBUS_LDAP_THR_LIBS) \
	$(BOOST_LIBS) \
        $(EXPAT_LIBS) \
	$(CLASSAD_DL_LIBS)

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(MYPROXY_CFLAGS) \
	$(GLOBUS_SSL_UTILS_CFLAGS) \
        -DGLITE_WMS_HAVE_LBPROXY

MAINTAINERCLEANFILES = Makefile.in
