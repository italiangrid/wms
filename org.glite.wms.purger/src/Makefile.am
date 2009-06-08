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

lib_LTLIBRARIES = libglite_wms_purger.la

incdir = $(includedir)/glite/wms/purger
nobase_inc_HEADERS = \
	purger.h 

sbin_PROGRAMS      = glite-wms-purgeStorage

#wlpurgerdir = $(sysconfdir)/cron
#wlpurger_DATA = edg-wl-purgeStorage-weekly.crontab \
#	edg-wl-purgeStorage-hourly.crontab 

#libexec_SCRIPTS = edg-wl-purgestorage.sh

#wlprdir = $(datadir)/doc/@PACKAGE@-wm-@VERSION@
#wlpr_DATA = README.purge

libglite_wms_purger_la_SOURCES = \
	purger.cpp \
	lb_utils.cpp \
        ssl_utils.cpp 

glite_wms_purgeStorage_SOURCES = \
	glite-wms-purgeStorage.cpp

glite_wms_purgeStorage_LDADD = \
        $(GLITE_LDFLAGS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_LB_NOTHR_CLIENT_LIBS) \
	$(GLITE_LB_NOTHR_CLIENTPP_LIBS) \
	$(GLITE_LB_NOTHR_COMMON_LIBS) \
	./libglite_wms_purger.la \
	$(BOOST_LIBS) \
	$(GLOBUS_SSL_NOTHR_LIBS) \
	$(GLOBUS_COMMON_NOTHR_LIBS) \
	$(GLOBUS_GSS_NOTHR_LIBS) \
	$(CLASSAD_LIBS) \
        $(GLITE_JOBID_LIBS)

AM_CPPFLAGS = \
	$(GLITE_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(GLOBUS_THR_CFLAGS)

#EXTRA_DIST = edg-wl-purgeStorage-weekly.crontab \
#       edg-wl-purgeStorage-hourly.crontab \
#	README.purge \
#	edg-wl-purgestorage.sh.in

#CLEANFILES = edg-wl-purgestorage.sh

MAINTAINERCLEANFILES = Makefile.in

