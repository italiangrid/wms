# Copyright (c) Members of the EGEE Collaboration. 2004.
# See http://www.eu-egee.org/partners/ for details on the copyright
# holders.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
	$(BOOST_FS_LIBS) $(BOOST_THREAD_LIBS) $(BOOST_REGEX_LIBS) \
	$(ARES_LIBS) \
	$(CLASSAD_LIBS) \
	$(CONDORG_LIBS) \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_JDL_LIBS) \
	$(GLITE_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_LB_CLIENT_LIBS) \
	$(GLITE_LB_CLIENTPP_LIBS) \
	$(GLITE_LB_COMMON_LIBS) \
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
