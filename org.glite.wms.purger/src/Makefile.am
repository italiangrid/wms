## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

noinst_LTLIBRARIES = libglite_wms_purger.la

noinst_HEADERS     = purger.h

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
	jp_upload_files.cpp

glite_wms_purgeStorage_SOURCES = \
	glite-wms-purgeStorage.cpp

glite_wms_purgeStorage_LDADD = \
	$(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_LB_NOTHR_CLIENT_LIBS) \
	$(GLITE_LB_NOTHR_CLIENTPP_LIBS) \
	$(GLITE_LB_NOTHR_COMMON_LIBS) \
	./libglite_wms_purger.la \
        $(GLITE_WMS_OPTIONAL_JP_LIBS) \
	$(BOOST_LIBS) \
	$(GLOBUS_SSL_NOTHR_LIBS) \
	$(GLOBUS_COMMON_NOTHR_LIBS) \
	$(GLOBUS_GSS_NOTHR_LIBS) \
	$(CLASSAD_LIBS) \
	$(ARES_LIBS) \
	$(EXPAT_LIBS) \
	-lstdc++ 

AM_CPPFLAGS = \
	$(GLITE_CFLAGS) \
	-I$(top_srcdir)/src \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	-DWITH_LDAP_EXCEPTIONS \
        $(GLITE_WMS_OPTIONAL_JP_CFLAGS)

#EXTRA_DIST = edg-wl-purgeStorage-weekly.crontab \
#       edg-wl-purgeStorage-hourly.crontab \
#	README.purge \
#	edg-wl-purgestorage.sh.in

#CLEANFILES = edg-wl-purgestorage.sh

MAINTAINERCLEANFILES = Makefile.in

