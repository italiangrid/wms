ifndef prefix
prefix=$(shell pwd)
endif

ifndef topdir
topdir=$(shell rpm --eval %_topdir)
endif

SITE=emi.wms
PKG1=wmsmetrics

SRPM=$(PKGNAME)-$(PKGVERS).src.tgz
SPECFILE=project/emi-wms-nagios.spec
PROBES=src/WMS-probe src/WMS-jdl.template
FILES=CHANGES setup.py README
MODS1=src/$(PKG1)/__init__.py \
    src/$(PKG1)/WmsMetrics.py \
    src/$(PKG1)/scheduler.py

buildcmd  := $(shell [ -x /usr/bin/rpmbuild ] && echo rpmbuild || echo rpm)

PKGNAME = $(shell grep -s '^Name:'    $(SPECFILE) | sed -e 's/Name: *//')
PKGVERS = $(shell grep -s '^Version:' $(SPECFILE) | sed -e 's/Version: *//')

distdir=$(prefix)/$(PKGNAME)-$(PKGVERS)
rpmtopdir=$(topdir)/rpmbuild

dist: $(SPECFILE) $(PROBES) $(FILES) $(MODS1) 
	mkdir -p $(distdir)/usr/libexec/grid-monitoring/probes/$(SITE)
	cp -rpf $(PROBES) $(distdir)/usr/libexec/grid-monitoring/probes/$(SITE)
	mkdir -p $(distdir)/$(PKG1) 
	cp -f $(MODS1) $(distdir)/$(PKG1)
	cp -f $(FILES) $(distdir)
	find $(distdir) -path '.*swp' -prune -exec rm -rf {} \;
	find $(distdir) -path 'CVS' -prune -exec rm -rf {} \;

bldprep: dist $(SPECFILE)
	mkdir -p $(rpmtopdir)/{SOURCES,SPECS,BUILD,SRPMS,RPMS}
	cd $(prefix) && tar czf ${rpmtopdir}/SOURCES/$(SRPM) --exclude='CVS' $(PKGNAME)-$(PKGVERS)/*
	cp -f $(SPECFILE) ${rpmtopdir}/SPECS/$(SPECFILE)

rpm: bldprep dist $(SPECFILE)
	$(buildcmd) --define '_topdir ${rpmtopdir}' -ba $(SPECFILE)

sources: dist
	cd $(prefix) && tar czf ../$(SRPM) --exclude='CVS' $(PKGNAME)-$(PKGVERS)/*
	rm -rf $(distdir)

clean:
	rm -rf $(distdir)
	rm -rf $(rpmtopdir)
	rm -rf $(SRPM)
