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
#       Revision 1.17.2.3.4.1.2.5.2.7  2012/06/20 13:44:33  pandreet
#       Used pkgconfig macro for condorg
#
#       Revision 1.17.2.3.4.1.2.5.2.6  2012/06/18 11:58:06  mcecchi
#       reintroduced id repository in JC. cleanup
#
#       Revision 1.17.2.3.4.1.2.5.2.5  2012/02/23 13:38:30  pandreet
#       Replaced macros with pkg files
#
#       Revision 1.17.2.3.4.1.2.5.2.4  2012/02/20 10:27:09  mcecchi
#       restored LM
#
#       Revision 1.17.2.3.4.1.2.5.2.3  2012/02/15 11:02:49  pandreet
#       Removed variables for globus
#
#       Revision 1.17.2.3.4.1.2.5.2.2  2012/02/07 14:36:30  mcecchi
#       removed support for filelist - work in progress
#
#       Revision 1.17.2.3.4.1.2.5.2.1  2011/09/12 09:25:38  pandreet
#       Fixed conflict with lb macro
#
#       Revision 1.17.2.3.4.1.2.5  2011/03/11 15:00:04  pandreet
#       Removed with-glite-location
#
#       Revision 1.17.2.3.4.1.2.4  2011/01/05 08:50:58  pandreet
#       New build definitions for jobsubmission
#
#       Revision 1.17.2.3.4.1.2.3  2010/09/03 10:11:46  pandreet
#       Ares AC definitions in build files
#
#       Revision 1.17.2.3.4.1.2.2  2010/02/16 15:26:01  fcapanni
#       wrong lib staging by lb
#
#       Revision 1.17.2.3.4.1.2.1  2009/06/16 13:15:22  fcapanni
#       porting to org.glite.jobid
#
#       Revision 1.17.2.3.4.1  2009/02/17 15:35:47  mcecchi
#       LB type run-time selection
#
#       Revision 1.17.2.3  2008/02/04 07:12:34  giaco
#       - remove deps from expat and libtar
#
#       Revision 1.17.2.2  2006/04/03 16:17:04  gianelle
#       emoved JP dependencies (GLITE_WMS_OPTIONAL_JP_LIBS)
#
#       Revision 1.17.2.1  2006/03/28 10:26:06  eronchie
#       THR moved in NOTHR for procyrenewal
#
#       Revision 1.17  2006/03/16 17:28:40  gianelle
#       Migration to "new" jdl library
#
#       Revision 1.16  2006/03/15 13:19:54  gianelle
#       Drops any explicit references to ares.
#
#       Revision 1.15  2006/03/15 12:55:51  gianelle
#       Added (macro) support for:
#       - libtar
#       - Optimization
#       - Gpbox
#       - JP
#       - Purger
#
#       Revision 1.14  2006/03/14 21:22:14  eronchie
#       Applied PURGER macro
#
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
