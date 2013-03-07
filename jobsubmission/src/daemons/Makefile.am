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

WL_JCTRL_DCTRL=glite-wms-job_controller
WL_JCTRL_DLOG=glite-wms-log_monitor

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
	$(GLITE_JOBID_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_JDL_LIBS)\
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMS_COMMON_PROCESS_LIBS) \
	$(GLITE_LB_CLIENT_LIBS) \
	$(GLITE_LB_CLIENTPP_LIBS) \
	$(GLITE_LB_COMMON_LIBS) \
	$(CLASSAD_LIBS) $(BOOST_LIBS) \
	$(ARES_LIBS) \
	$(CONDORG_LIBS) \
	$(RENEWAL_LIBS) \
	$(GLITE_WMS_PURGER_LIBS) \
	$(GLITE_JOBID_LIBS)

glite_wms_log_monitor_SOURCES = logmonitor.cpp \
	MonitorLoop.cpp \
	exceptions.cpp

glite_wms_log_monitor_LDADD = ../controller/libglite_wms_jss_controller.la \
	../controller/libglite_wms_jss_controller_adapter.la \
	../logmonitor/libglite_wms_jss_logmonitor.la \
	../common/libglite_wms_jss_common.la \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_JOBID_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_JDL_LIBS)\
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMS_COMMON_PROCESS_LIBS) \
	$(GLITE_LB_CLIENT_LIBS) \
	$(GLITE_LB_CLIENTPP_LIBS) \
	$(GLITE_LB_COMMON_LIBS) \
	$(CLASSAD_LIBS) $(BOOST_LIBS) \
	$(ARES_LIBS) \
	$(CONDORG_LIBS) \
        $(RENEWAL_LIBS) \
	$(GLITE_WMS_PURGER_LIBS) \
	$(GLITE_JOBID_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GLITE_JOBID_CFLAGS) \
	$(GLITE_WMS_COMMON_CONF_CFLAGS) \
	$(GLITE_WMS_COMMON_LOGGER_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(CONDORG_CFLAGS) -DCONDORG_VERSION=$(CONDORG_VERSION)

MAINTAINERCLEANFILES = Makefile.in *~
