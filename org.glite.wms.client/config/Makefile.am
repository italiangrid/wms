## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

wletcdir = $(sysconfdir)
wletc_SCRIPTS = glite-wmsclient-vars.sh glite-wmsclient-vars.csh

wletcutilsdir = $(sysconfdir)/profile.d
wletcutils_SCRIPTS = glite-wmsclient.sh glite-wmsclient.csh 

wletcwmdir = $(sysconfdir)/vo_template
wletcwm_DATA = glite_wmsclient.conf

install-data-hook:
	/bin/mv $(DESTDIR)$(sysconfdir)/vo_template/glite_wmsclient.conf $(DESTDIR)$(sysconfdir)/vo_template/glite_wmsclient.conf.template

EXTRA_DIST = \
        glite_wmsclient.conf 

CLEANFILES = \
	glite-wmsclient.csh

MAINTAINERCLEANFILES = Makefile.in *~ *.*~

