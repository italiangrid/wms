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
#       Revision 1.17  2006/03/14 21:22:14  eronchie
#       Applied PURGER macro
#
#       Revision 1.16  2006/01/24 12:35:55  gianelle
#       Added some new condor events:
#        EventGridResourceDown
#        EventGridResourceUp
#        EventGridSubmit
#
#       Added the support for condorG version 6.7.15
#
#       Revision 1.15  2005/12/06 12:53:28  gianelle
#       Change the name of some "ifdef" parameters:
#       ENABLE_LBPROXY --> GLITE_WMS_HAVE_LBPROXY
#       ENABLE_LOGGING --> GLITE_WMS_HAVE_LOGGING
#       ENABLE_RENEWAL --> GLITE_WMS_HAVE_RENEWAL
#       ENABLE_PURGER  --> GLITE_WMS_HAVE_PURGER
#
#       Revision 1.14  2005/11/15 10:19:55  gianelle
#       Added LBProxy macro setting
#
#       Revision 1.13  2005/09/26 13:17:40  gianelle
#       Added RENEWAL_NOTHR_LIBS library.
#       Commented unused function.
#
#       Revision 1.12  2004/10/13 14:27:57  eronchie
#       Removed sslutils dependency
#
#       Revision 1.11  2004/09/07 10:21:22  gianelle
#       Fixes some LCG2 bugs:
#       3807 - Log monitor must not crash on bad CondorG log files
#       3883 - Cancel request from JC never appears in condorG
#       3884 - Jobs not cancelled: unknown to the system
#       3895 - Job canceled on receipt of ULOG_GLOBUS_RESOURCE_DOWN
#       3987 - Try to finish job exiting with globus error 155
#
#       Fixed a new bug found during the test of the previous patches:
#       a user cancel during the timeout resubmission failed (the job was
#       resubmitted in any case)
#
#       Revision 1.10  2004/08/24 14:06:32  pandreet
#       GSSAPI dependencies
#
#       Revision 1.9  2004/08/12 17:20:31  eronchie
#       Changed renewal in security
#       Changed NOTHR_RENEWAL_LIBS in RENEWAL_NOTHR_LIBS
#
#       Revision 1.8  2004/08/10 12:30:04  eronchie
#       Added jss
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
#       Revision 1.2  2004/07/25 17:40:02  eronchie
#       Updated condor prefix
#
#       Revision 1.1  2004/07/06 10:05:38  pandreet
#       Basic build files for jobsubmission
#

lib_LTLIBRARIES = libglite_wms_jss_logmonitor.la

noinst_HEADERS = \
	JobStatusExtractor.h \
	JobWrapperOutputParser.h \
	AbortedContainer.h \
	CondorMonitor.h \
	EventAd.h \
	exceptions.h \
	SizeFile.h \
	Timer.h

#GLITE_WMS_PURGER_LIBS = $(top_srcdir)/../org.glite.wms.purger/build/src/libglite_wms_purger.la


SUBDIRS = processer

libexec_PROGRAMS = glite-wms-lm-job_status

glite_wms_lm_job_status_SOURCES = jobstatus.cpp \
	JobStatusExtractor.cpp
glite_wms_lm_job_status_LDADD = libglite_wms_jss_logmonitor.la \
	../controller/libglite_wms_jss_controller.la \
	../controller/libglite_wms_jss_controller_adapter.la \
	../common/libglite_wms_jss_common.la \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMS_JDL_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(BOOST_FS_LIBS) $(BOOST_THREAD_LIBS) $(BOOST_REGEX_LIBS) \
	$(GLOBUS_LIBS) $(GLOBUS_SSL_NOTHR_LIBS) \
	$(GLOBUS_GSS_NOTHR_LIBS) \
	$(GLOBUS_COMMON_NOTHR_LIBS) \
	$(CLASSAD_LIBS) $(CONDORG_LIBS) \
	$(GLITE_LB_NOTHR_CLIENT_LIBS) $(GLITE_LB_NOTHR_CLIENTPP_LIBS) \
	$(GLITE_LB_NOTHR_COMMON_LIBS) \
	$(ARES_LIBS) $(EXPAT_LIBS) \
	$(GLITE_SECURITY_RENEWAL_NOTHR_LIBS) $(GLITE_WMS_PURGER_LIBS) \
	$(RENEWAL_NOTHR_LIBS)	

libglite_wms_jss_logmonitor_la_SOURCES = CondorMonitor.cpp \
	EventAd.cpp \
	Timer.cpp \
	SizeFile.cpp \
	AbortedContainer.cpp \
	JobWrapperOutputParser.cpp \
	exceptions.cpp \
	./processer/EventFactory.cpp \
	./processer/EventInterface.cpp \
	./processer/EventUnhandled.cpp \
	./processer/EventSubmit.cpp \
	./processer/EventGlobusSubmit.cpp \
	./processer/EventGridSubmit.cpp \
	./processer/EventExecute.cpp \
	./processer/EventTerminated.cpp \
	./processer/EventPostTerminated.cpp \
	./processer/EventAborted.cpp \
	./processer/EventGlobusSubmitFailed.cpp \
	./processer/EventGlobusResourceDown.cpp \
	./processer/EventGridResourceDown.cpp \
	./processer/EventGlobusResourceUp.cpp \
	./processer/EventGridResourceUp.cpp \
	./processer/EventJobHeld.cpp \
	./processer/EventGeneric.cpp \
	./processer/MonitorData.cpp \
	./processer/SubmitReader.cpp \
	./processer/JobResubmitter.cpp

libglite_wms_jss_logmonitor_la_LIBADD = $(CONDORG_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(GLOBUS_NOTHR_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(CONDORG_CFLAGS) -DGLITE_WMS_HAVE_LBPROXY 

MAINTAINERCLEANFILES = Makefile.in *~
