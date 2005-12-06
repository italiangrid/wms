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
#       Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
#       Version info: $Id$
#       Release: $Name$
#
#       Revision history:
#       $Log$
#       Revision 1.13  2005/12/06 12:53:28  gianelle
#       Change the name of some "ifdef" parameters:
#       ENABLE_LBPROXY --> GLITE_WMS_HAVE_LBPROXY
#       ENABLE_LOGGING --> GLITE_WMS_HAVE_LOGGING
#       ENABLE_RENEWAL --> GLITE_WMS_HAVE_RENEWAL
#       ENABLE_PURGER  --> GLITE_WMS_HAVE_PURGER
#
#       Revision 1.12  2005/09/20 14:11:37  gianelle
#       Syncronize the rc1 branch with head.
#
#       Revision 1.11  2004/10/13 14:27:57  eronchie
#       Removed sslutils dependency
#
#       Revision 1.10  2004/10/06 11:28:39  pandreet
#       Bug fixed: missing GLITE_WMSUTILS_THP_GLOBUS_SSL_UTILS_LIBS
#
#       Revision 1.9  2004/08/24 14:06:32  pandreet
#       GSSAPI dependencies
#
#       Revision 1.8  2004/08/17 12:41:36  eronchie
#       Updated RENEWAL check
#
#       Revision 1.7  2004/08/10 12:01:43  eronchie
#       Updated
#
#       Revision 1.6  2004/07/28 14:38:58  pandreet
#       Proxyrenewal dependencies fixed
#
#       Revision 1.5  2004/07/28 13:51:38  pandreet
#       Purger dependencies fixed
#
#       Revision 1.4  2004/07/28 10:22:36  eronchie
#       Added the prefix GLITE_WMSUTILS_THP_ in the macro GLOBUS_SSL_UTILS
#
#       Revision 1.3  2004/07/27 13:46:37  pandreet
#       LB dependencies fixed
#
#       Revision 1.2  2004/07/27 09:41:26  pandreet
#       JobId and Exception dependencies fixed
#
#       Revision 1.1  2004/07/06 10:05:38  pandreet
#       Basic build files for jobsubmission
#

WL_JCTRL_DCTRL=glite-wms-job_controller
WL_JCTRL_DLOG=glite-wms-log_monitor

GLITE_WMS_PURGER_LIBS = $(top_srcdir)/../org.glite.wms.purger/build/src/libglite_wms_purger.la

bin_PROGRAMS = $(WL_JCTRL_DCTRL) $(WL_JCTRL_DLOG)

noinst_HEADERS = \
	ControllerLoop.h \
	exceptions.h \
	MonitorLoop.h

glite_wms_job_controller_SOURCES = jobcontroller.cpp \
	ControllerLoop.cpp \
	exceptions.cpp
glite_wms_job_controller_LDADD = ../controller/libglite_wms_jss_controller.la \
	../controller/libglite_wms_jss_controller_adapter.la \
	../common/libglite_wms_jss_common.la \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMS_JDL_LIBS)\
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMS_COMMON_PROCESS_LIBS) \
	$(GLITE_LB_NOTHR_CLIENT_LIBS) $(GLITE_LB_NOTHR_CLIENTPP_LIBS) \
	$(GLITE_LB_NOTHR_COMMON_LIBS) \
	$(GLOBUS_LIBS) $(GLOBUS_SSL_NOTHR_LIBS) \
	$(GLOBUS_GSS_NOTHR_LIBS) \
	$(GLOBUS_COMMON_NOTHR_LIBS) \
	$(CLASSAD_LIBS) $(BOOST_LIBS) \
	$(ARES_LIBS) $(EXPAT_LIBS) $(CONDORG_LIBS) \
	$(GLITE_SECURITY_RENEWAL_THR_LIBS) \
	$(GLITE_WMS_PURGER_LIBS)

glite_wms_log_monitor_SOURCES = logmonitor.cpp \
	MonitorLoop.cpp \
	exceptions.cpp
glite_wms_log_monitor_LDADD = ../controller/libglite_wms_jss_controller.la \
	../controller/libglite_wms_jss_controller_adapter.la \
	../logmonitor/libglite_wms_jss_logmonitor.la \
	../common/libglite_wms_jss_common.la \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMS_JDL_LIBS)\
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMS_COMMON_PROCESS_LIBS) \
	$(GLITE_LB_NOTHR_CLIENT_LIBS) $(GLITE_LB_NOTHR_CLIENTPP_LIBS) \
	$(GLITE_LB_NOTHR_COMMON_LIBS) \
	$(GLOBUS_LIBS) $(GLOBUS_SSL_NOTHR_LIBS) \
	$(GLOBUS_GSS_NOTHR_LIBS) \
	$(GLOBUS_COMMON_NOTHR_LIBS) \
	$(CLASSAD_LIBS) $(BOOST_LIBS) \
	$(CONDORG_LIBS) $(ARES_LIBS) $(EXPAT_LIBS) \
        $(GLITE_SECURITY_RENEWAL_THR_LIBS) \
	$(GLITE_WMS_PURGER_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(GLOBUS_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(ARES_CFLAGS) $(EXPAT_CFLAGS) \
	$(CONDORG_CFLAGS) -DGLITE_WMS_HAVE_LBPROXY

MAINTAINERCLEANFILES = Makefile.in *~
