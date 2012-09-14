## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

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
	lb_utils.cpp
libglite_wms_purger_la_LDFLAGS = $(GLITE_LDFLAGS)
libglite_wms_purger_la_LIBADD = $(GLITE_WMS_COMMON_LIBS)

glite_wms_purgeStorage_SOURCES = \
	glite-wms-purgeStorage.cpp
glite_wms_purgeStorage_LDFLAGS = $(GLITE_LDFLAGS) $(CLASSAD_LDFLAGS)
glite_wms_purgeStorage_LDADD = \
	./libglite_wms_purger.la \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_LB_CLIENTPP_NOTHR_LIBS) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

AM_CPPFLAGS = \
	$(GLITE_CPPFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CPPFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	-DWITH_LDAP_EXCEPTIONS

#EXTRA_DIST = edg-wl-purgeStorage-weekly.crontab \
#       edg-wl-purgeStorage-hourly.crontab \
#	README.purge \
#	edg-wl-purgestorage.sh.in

#CLEANFILES = edg-wl-purgestorage.sh

MAINTAINERCLEANFILES = Makefile.in

