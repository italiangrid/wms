## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wms_manager_server.la 

libexec_SCRIPTS = glite-wms-dag-post.sh

libexec_PROGRAMS = glite-wms-planner glite-wms-planner_real

noinst_PROGRAMS = \
	glite-wms_input_generator

noinst_HEADERS = \
        RequestHandler.h \
	signal_handling.h \
	listmatch.h \
	WMReal.h

libglite_wms_manager_server_la_SOURCES = \
	listmatch.h \
	listmatch.cpp\
	Request.hpp \
	Request.cpp \
	TaskQueue.hpp \
	plan.h \
	plan.cpp \
	DAGManHelper.h \
	DAGManHelper.cpp \
	dispatcher.h \
	dispatcher.cpp \
	RequestHandler.h \
	RequestHandler.cpp \
	WMReal.h \
	WMReal.cpp \
	signal_handling.h \
	signal_handling.cpp \
	match_utils.h \
	match_utils.cpp \
        lb_utils.h \
        lb_utils.cpp \
	dynamic_library.h \
	dynamic_library.cpp \
	submission_utils.h \
	submission_utils.cpp \
	input_reader.h \
	input_reader.cpp \
	filelist_reader.h \
	filelist_reader.cpp \
	jobdir_reader.h \
	jobdir_reader.cpp

# 	filelist_recovery.h \
# 	filelist_recovery.cpp \
#

glite_wms_planner_SOURCES = \
        glite-process-counter-args.c

glite_wms_planner_real_SOURCES = glite-wms-planner.cpp
glite_wms_planner_real_LDFLAGS = -Xlinker --export-dynamic
glite_wms_planner_real_LDADD   = \
	libglite_wms_manager_server.la \
	$(GLITE_WMS_HELPER_LIBS) \
	$(GLITE_WMS_HELPER_JOBADAPTER_LIBS) \
	$(GLITE_WMS_PURGER_LIBS) \
	$(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_JDL_LIBS)\
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMS_COMMON_UT_II_LIBS) \
	$(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMS_JSS_CONTROLLER_ADAPTER_LIBS) \
	$(GLITE_WMS_JSS_COMMON_LIBS) \
	$(GLITE_LB_THR_CLIENT_LIBS) \
	$(GLITE_LB_THR_CLIENTPP_LIBS) \
	$(GLITE_LB_THR_COMMON_LIBS) \
	$(GLITE_SECURITY_RENEWAL_THR_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS)

glite_wms_input_generator_SOURCES = \
	input_generator.cpp
glite_wms_input_generator_LDADD = \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_JDL_LIBS) \
	$(GLITE_LB_THR_CLIENT_LIBS) \
	$(GLITE_LB_THR_COMMON_LIBS) \
	$(GLITE_SECURITY_RENEWAL_THR_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS)

AM_CPPFLAGS = \
	$(GLITE_CFLAGS) \
        $(CLASSAD_CFLAGS) \
        $(BOOST_CFLAGS)

EXTRA_DIST = glite-wms-dag-post.sh.in

CLEANFILES = glite-wms-dag-post.sh

MAINTAINERCLEANFILES = Makefile.in
