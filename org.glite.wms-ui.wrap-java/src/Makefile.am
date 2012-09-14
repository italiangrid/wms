## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wmsui_native.la

GLITE_WMS_MANAGER_NS_CLIENT_LIBS = -L$(top_srcdir)/../org.glite.wms.manager-ns-client/build/src \
        -L$(top_srcdir)/../org.glite.wms.manager-ns-commands/build/src \
        -L$(top_srcdir)/../org.glite.wms.manager-ns-fsm/build/src \
        -L$(top_srcdir)/../org.glite.wms.manager-ns-versions/build/src \
        -lglite_wms_manager_ns_client -lnsclientcommands -lnsfsm -lglite_wms_manager-ns-versions

libglite_wmsui_native_la_SOURCES = \
	org_glite_wmsui_apij_Api.h \
	org_glite_wmsui_apij_Api.cpp 
libglite_wmsui_native_la_LIBADD = \
        -lc \
	$(GLITE_WMS_MANAGER_NS_CLIENT_LIBS) \
	$(VOMS_CPP_NOTHR_LIBS) \
	$(GLITE_WMSUTILS_TLS_GSI_SOCKET_LIBS) \
        $(GLITE_WMSUTILS_CJOBID_LIBS) \
        $(GLITE_WMSUTILS_JOBID_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_JDL_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMS_COMMON_UT_FTP_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_LB_NOTHR_CLIENT_LIBS) \
        $(GLITE_LB_NOTHR_CLIENTPP_LIBS) \
        $(GLITE_LB_NOTHR_COMMON_LIBS) \
	$(GLITE_DGAS_JOBAUTHCLIENT_LIBS) \
	$(GLITE_DGAS_COMMON_XMLUTIL_LIBS) \
	$(CLASSAD_LIBS) \
	$(ARES_LIBS) \
	$(BOOST_LIBS) \
	$(GLOBUS_COMMON_NOTHR_LIBS) \
	$(GLOBUS_SSL_NOTHR_LIBS) \
	$(GLOBUS_SSL_NOTHR_LIBS) \
	$(GLOBUS_GSS_NOTHR_LIBS) \
        $(GLOBUS_FTP_CLIENT_NOTHR_LIBS) \
        $(EXPAT_LIBS) \
	$(GLITE_WMS_PARTITIONER_LIBS) \
	$(GLITE_WMS_CHKPT_LIBS) \
	-lstdc++ 

org_glite_wmsui_apij_Api.lo: org_glite_wmsui_apij_Api.cpp
	/bin/touch org_glite_wmsui_apij_Api.h
	javah -jni \
	-d . -classpath $(prefix)/share/java/glite-wms-ui-api-java.jar \
	org.glite.wmsui.apij.Api
	/usr/bin/libtool $(CXX) -c $(CFLAGS) -DHAVE_CONFIG_H \
	-I. -I. -I../.. $(AM_CPPFLAGS) \
	-o org_glite_wmsui_apij_Api.o $(top_srcdir)/src/org_glite_wmsui_apij_Api.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src \
	-I$(top_builddir)/src/autogen \
	-I$(top_srcdir)/../org.glite.wms.manager-ns-client/src \
	$(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(GLOBUS_NOTHR_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(EXPAT_CFLAGS) \
	$(ARES_CFLAGS) \
	$(JAVA_CFLAGS) \
	-DWANT_NAMESPACES

CLEANFILES = org_glite_wmsui_apij_Api.h

MAINTAINERCLEANFILES = Makefile.in
