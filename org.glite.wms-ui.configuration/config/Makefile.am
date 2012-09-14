## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

wletcdir = $(sysconfdir)
wletc_SCRIPTS = glite-wmsui-vars.sh glite-wmsui-vars.csh

wletcutilsdir = $(sysconfdir)/profile.d
wletcutils_SCRIPTS = glite-wmsui.sh glite-wmsui.csh \
	glite-wmsui-env.csh glite-wmsui-env.sh \
        glite-wmsui-gui-env.csh glite-wmsui-gui-env.sh

wletcwmdir = $(sysconfdir)/vo_template
wletcwm_DATA = glite_wmsui.conf

install-data-hook:
	/bin/mv $(DESTDIR)$(sysconfdir)/vo_template/glite_wmsui.conf $(DESTDIR)$(sysconfdir)/vo_template/glite_wmsui.conf.template

EXTRA_DIST = \
        glite_wmsui.conf \
	glite-wmsui.csh.in \
	glite-wmsui-env.csh.in \
	glite-wmsui-env.sh.in \
	glite-wmsui-gui-env.csh.in \
	glite-wmsui-gui-env.sh.in \
	glite-wmsui.sh.in \
	glite-wmsui.csh.in \
	glite-wmsui-vars.csh.in \
	glite-wmsui-vars.sh.in

CLEANFILES = \
	glite-wmsui.csh \
        glite-wmsui-env.csh \
        glite-wmsui-env.sh \
        glite-wmsui-gui-env.csh \
        glite-wmsui-gui-env.sh \
        glite-wmsui.sh \
	glite-wmsui.csh \
        glite-wmsui-vars.csh \
        glite-wmsui-vars.sh

MAINTAINERCLEANFILES = Makefile.in *~ *.*~

