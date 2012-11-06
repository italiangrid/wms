libexec_SCRIPTS = \
   glite-wms-check-daemons.sh \
   glite-wms-services-certs.sh \
   glite-wms-parse-configuration.sh

wletcwmdir = $(sysconfdir)/glite-wms
wletcwm_DATA = glite_wms.conf

install-data-hook:
	/bin/mkdir -p $(prefix)/../opt/glite/yaim/functions/ $(prefix)/../opt/glite/yaim/services/ $(prefix)/../opt/glite/yaim/defaults/ $(prefix)/../opt/glite/yaim/node-info.d/ $(prefix)/share/man/man1/
	/bin/cp $(top_srcdir)/config/functions/config_* $(prefix)/../opt/glite/yaim/functions/
	/bin/cp $(top_srcdir)/config/services/glite-wms $(prefix)/../opt/glite/yaim/services/
	/bin/cp $(top_srcdir)/config/defaults/glite-wms.* $(prefix)/../opt/glite/yaim/defaults/
	/bin/cp $(top_srcdir)/config/node-info.d/glite-*  $(prefix)/../opt/glite/yaim/node-info.d/
	/bin/cp $(top_srcdir)/config/man/glite-WMS.1* $(prefix)/share/man/man1/
	/bin/mv $(wletcwmdir)/glite_wms.conf $(wletcwmdir)/wms.conf.template

MAINTAINERCLEANFILES = Makefile.in *~ *.*~
