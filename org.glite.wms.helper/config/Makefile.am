## *********************************************************************
## *
## * Author: Elisabetta Ronchieri <elisabetta.ronchieri@cnaf.infn.it>
## *
## *********************************************************************


helperconfigdir = $(datadir)/glite-wms
helperconfig_DATA = \
	template.sh

install-data-hook:
	/bin/mv $(DESTDIR)$(helperconfigdir)/template.sh $(DESTDIR)$(helperconfigdir)/jobwrapper.template.sh

MAINTAINERCLEANFILES = Makefile.in *~ *.*~
