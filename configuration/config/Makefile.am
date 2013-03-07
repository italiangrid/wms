libexec_SCRIPTS = \
   glite-wms-check-daemons.sh \
   glite-wms-services-certs.sh \
   glite-wms-parse-configuration.sh

wletcwmdir = $(sysconfdir)/glite-wms
wletcwm_DATA = glite_wms.conf

install-data-hook:
	@echo installing ...
	@mkdir -p $(prefix)/../opt/glite/yaim/functions/
	@mkdir -p $(prefix)/../opt/glite/yaim/functions/local
	@mkdir -p $(prefix)/../opt/glite/yaim/node-info.d
	@mkdir -p $(prefix)/../opt/glite/yaim/services
	@mkdir -p $(prefix)/../opt/glite/yaim/defaults
	@mkdir -p ${prefix}/../opt/glite/share/man/man1
	@install -m 0644 $(top_srcdir)/config/functions/config* $(prefix)/../opt/glite/yaim/functions
	@install -m 0644 $(top_srcdir)/config/node-info.d/glite* $(prefix)/../opt/glite/yaim/node-info.d
	@install -m 0644 $(top_srcdir)/config/defaults/glite* $(prefix)/../opt/glite/yaim/defaults
	@install -m 0644 $(top_srcdir)/config/man/glite* ${prefix}/../opt/glite/share/man/man1
	@install -m 0644 $(top_srcdir)/config/services/glite* $(prefix)/../opt/glite/yaim/services
	@gzip ${prefix}/../opt/glite/share/man/man1/glite*
	/bin/mv $(wletcwmdir)/glite_wms.conf $(wletcwmdir)/wms.conf.template

MAINTAINERCLEANFILES = Makefile.in *~ *.*~
