## Copyright (c) Members of the EGEE Collaboration. 2009. 
## See http://www.eu-egee.org/partners/ for details on the copyright holders.  

## Licensed under the Apache License, Version 2.0 (the "License"); 
## you may not use this file except in compliance with the License. 
## You may obtain a copy of the License at 
##     http://www.apache.org/licenses/LICENSE-2.0 
## Unless required by applicable law or agreed to in writing, software 
## distributed under the License is distributed on an "AS IS" BASIS, 
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
## See the License for the specific language governing permissions and 
## limitations under the License.
#
#       Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
#       Version info: $Id$
#       Release: $Name$
#
#       Revision history:
#       $Log$
#       Revision 1.20.2.2.4.1.2.7.2.7  2012/06/20 13:44:34  pandreet
#       Used pkgconfig macro for condorg
#
#       Revision 1.20.2.2.4.1.2.7.2.6  2012/06/18 11:58:06  mcecchi
#       reintroduced id repository in JC. cleanup
#
#       Revision 1.20.2.2.4.1.2.7.2.5  2012/02/23 13:38:30  pandreet
#       Replaced macros with pkg files
#
#       Revision 1.20.2.2.4.1.2.7.2.4  2012/02/15 10:38:00  pandreet
#       Replaced macro for globus
#
#       Revision 1.20.2.2.4.1.2.7.2.3  2012/02/10 16:17:07  mcecchi
#       filelist in isolated JC/LM locations. possible future redesign
#
#       Revision 1.20.2.2.4.1.2.7.2.2  2012/02/07 14:36:30  mcecchi
#       removed support for filelist - work in progress
#
#       Revision 1.20.2.2.4.1.2.7.2.1  2011/09/12 09:25:38  pandreet
#       Fixed conflict with lb macro
#
#       Revision 1.20.2.2.4.1.2.7  2011/03/11 15:00:04  pandreet
#       Removed with-glite-location
#
#       Revision 1.20.2.2.4.1.2.6  2011/01/05 08:50:58  pandreet
#       New build definitions for jobsubmission
#
#       Revision 1.20.2.2.4.1.2.5  2010/09/03 10:11:46  pandreet
#       Ares AC definitions in build files
#
#       Revision 1.20.2.2.4.1.2.4  2010/02/16 15:23:28  fcapanni
#       wrong lib staging by lb
#
#       Revision 1.20.2.2.4.1.2.3  2010/01/15 12:33:43  mcecchi
#       bug #48636
#
#       Revision 1.20.2.2.4.1.2.2  2009/09/11 13:59:47  mcecchi
#       bug #55452 + some compilation warnings removed
#
#       Revision 1.20.2.2.4.1.2.1  2009/06/16 13:40:49  fcapanni
#       porting to org.glite.jobid
#
#       Revision 1.20.2.2.4.1  2009/02/17 15:35:47  mcecchi
#       LB type run-time selection
#
#       Revision 1.20.2.2  2008/02/04 07:12:34  giaco
#       - remove deps from expat and libtar
#
#       Revision 1.20.2.1  2006/04/03 16:17:04  gianelle
#       emoved JP dependencies (GLITE_WMS_OPTIONAL_JP_LIBS)
#
#       Revision 1.20  2006/03/16 17:28:40  gianelle
#       Migration to "new" jdl library
#
#       Revision 1.19  2006/03/15 13:19:55  gianelle
#       Drops any explicit references to ares.
#
#       Revision 1.18  2006/03/15 12:55:51  gianelle
#       Added (macro) support for:
#       - libtar
#       - Optimization
#       - Gpbox
#       - JP
#       - Purger
#
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

SUBDIRS = processer

libexec_PROGRAMS = glite-wms-lm-job_status

glite_wms_lm_job_status_SOURCES = jobstatus.cpp \
				JobStatusExtractor.cpp

glite_wms_lm_job_status_LDADD = libglite_wms_jss_logmonitor.la \
	../controller/libglite_wms_jss_controller.la \
	../controller/libglite_wms_jss_controller_adapter.la \
	../common/libglite_wms_jss_common.la \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_JOBID_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_JDL_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(BOOST_FS_LIBS) $(BOOST_THREAD_LIBS) $(BOOST_REGEX_LIBS) \
	$(CLASSAD_LIBS) \
	$(CONDORG_LIBS) \
	$(ARES_LIBS) \
	$(GLITE_LB_CLIENT_LIBS) \
	$(GLITE_LB_CLIENTPP_LIBS) \
	$(GLITE_LB_COMMON_LIBS) \
	$(RENEWAL_LIBS) \
	$(GLITE_WMS_PURGER_LIBS) \
        $(RENEWAL_LIBS) \
	$(GLITE_JOBID_LIBS)

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
	./processer/EventJobReleased.cpp \
	./processer/EventGeneric.cpp \
	./processer/MonitorData.cpp \
	./processer/SubmitReader.cpp \
	./processer/JobResubmitter.cpp

libglite_wms_jss_logmonitor_la_LIBADD = ../common/libglite_wms_jss_common.la \
					$(CONDORG_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/src \
	-I/usr/include \
	$(GLITE_JOBID_CFLAGS) \
	$(GLITE_WMS_COMMON_UT_UTIL_CFLAGS) \
	$(GLITE_WMS_COMMON_CONF_CFLAGS) \
	$(GLOBUS_GRAM_PROT_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(CONDORG_CFLAGS) -DCONDORG_VERSION=$(CONDORG_VERSION)

MAINTAINERCLEANFILES = Makefile.in *~
