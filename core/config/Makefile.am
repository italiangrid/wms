## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

EXTRA_DIST = \
	glite-wms-wm.in

wmdir = $(sysconfdir)/$(initdir)
wm_SCRIPTS = glite-wms-wm

configdir = $(datadir)/glite-wms
config_DATA = \
	template.sh

install-data-hook:
	/bin/mv $(DESTDIR)$(configdir)/template.sh $(DESTDIR)$(configdir)/jobwrapper.template.sh

MAINTAINERCLEANFILES = Makefile.in *~ *.*~
