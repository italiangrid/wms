## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

libexec_SCRIPTS = \
	glite-wms-check-daemons.sh \
	glite-wms-services-certs.sh \
	glite-wms-parse-configuration.sh

wletcwmdir = $(sysconfdir)/glite-wms
wletcwm_DATA = glite_wms.conf

install-data-hook:
	/bin/mv $(DESTDIR)$(wletcwmdir)/glite_wms.conf $(DESTDIR)$(wletcwmdir)/wms.conf.template

MAINTAINERCLEANFILES = Makefile.in *~ *.*~

