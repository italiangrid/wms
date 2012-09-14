## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

bin_PROGRAMS = \
        glite-wms-workload_manager

glite_wms_workload_manager_SOURCES = workload_manager.cpp ism_utils.cpp
glite_wms_workload_manager_LDFLAGS = -Xlinker --export-dynamic
glite_wms_workload_manager_LDADD   = \
	$(top_srcdir)/src/server/libglite_wms_manager_server.la
	$(GLITE_WMS_HELPER_LIBS) \
	-lglite-sd-c \
	$(GLITE_WMS_HELPER_JOBADAPTER_LIBS) \
	$(GLITE_WMS_ISM_LIBS) \
	${GLITE_WMS_PURGER_LIBS} \
	$(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_JDL_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMS_COMMON_UT_II_LIBS) \
	$(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_LB_THR_CLIENT_LIBS) \
	$(GLITE_LB_THR_CLIENTPP_LIBS) \
	$(GLITE_LB_THR_COMMON_LIBS) \
	$(GLITE_SECURITY_RENEWAL_THR_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS)

AM_CPPFLAGS = \
	$(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(GLOBUS_THR_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in
