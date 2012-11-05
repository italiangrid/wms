
libexec_SCRIPTS = \
   glite-wms-check-daemons.sh \
   glite-wms-services-certs.sh \
   glite-wms-parse-configuration.sh

wletcwmdir = $(sysconfdir)/glite-wms
wletcwm_DATA = glite_wms.conf

install-data-hook:
   /bin/mv $(DESTDIR)$(wletcwmdir)/glite_wms.conf $(DESTDIR)$(wletcwmdir)/wms.conf.template

MAINTAINERCLEANFILES = Makefile.in *~ *.*~
