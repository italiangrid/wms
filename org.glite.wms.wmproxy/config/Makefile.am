## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

EXTRA_DIST = \
	glite_wms_wmproxy_httpd.conf \
        glite_wms_wmproxy.gacl \
	glite-wms-wmproxy

wletcwmdir = $(sysconfdir)/glite-wms
wletcwm_DATA = glite_wms_wmproxy_httpd.conf \
		glite_wms_wmproxy.gacl

wletcwmsdir = $(sysconfdir)/lcmaps
wletcwms_DATA = lcmaps.db 

wlinidir = $(sysconfdir)/$(initdir)
wlini_SCRIPTS = glite-wms-wmproxy

install-data-hook:
	/bin/mv $(DESTDIR)$(wletcwmdir)/glite_wms_wmproxy_httpd.conf $(DESTDIR)$(wletcwmdir)/wmproxy_httpd.conf.template
	/bin/mv $(DESTDIR)$(wletcwmdir)/glite_wms_wmproxy.gacl $(DESTDIR)$(wletcwmdir)/wmproxy.gacl.template
	/bin/mv $(DESTDIR)$(wletcwmsdir)/lcmaps.db $(DESTDIR)$(wletcwmsdir)/lcmaps.db.template

CLEANFILES =

MAINTAINERCLEANFILES = Makefile.in *~ *.*~

