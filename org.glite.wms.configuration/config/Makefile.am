## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

wletcdir = $(sysconfdir)
wletc_SCRIPTS = glite-wms-vars.sh glite-wms-vars.csh
wletcutilsdir = $(sysconfdir)/profile.d
wletcutils_SCRIPTS = glite-wms.sh glite-wms.csh

libexec_SCRIPTS = \
	glite-wms-check-daemons.sh \
	glite-wms-services-certs.sh \
	glite-wms-parse-configuration.sh

wletcwmdir = $(sysconfdir)
wletcwm_DATA = glite_wms.conf

wletcwmsdir = $(sysconfdir)/profile.d
wletcwms_SCRIPTS = glite-wms-config.sh

install-data-hook:
	/bin/mv $(DESTDIR)$(sysconfdir)/glite_wms.conf $(DESTDIR)$(sysconfdir)/glite_wms.conf.template

EXTRA_DIST = \
        glite_wms.conf \
        glite-wms.csh.in \
        glite-wms.sh.in \
        glite-wms-vars.sh.in \
        glite-wms-vars.csh.in \
	glite-wms-config.sh.in \
	glite-wms-parse-configuration.sh.in \
	glite-wms-check-daemons.sh.in \
	glite-wms-services-certs.sh

CLEANFILES = \
	glite-wms.csh \
	glite-wms-parse-configuration.sh \
	glite-wms.sh \
	glite-wms-vars.sh \
	glite-wms-vars.csh \
	glite-wms-check-daemons.sh \
	glite-wms-config.sh 

MAINTAINERCLEANFILES = Makefile.in *~ *.*~

