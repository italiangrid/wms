#
#       Copyright (c) 2004 on behalf of the EU EGEE Project:
#       The European Organization for Nuclear Research (CERN),
#       Istituto Nazionale di Fisica Nucleare (INFN), Italy
#       Datamat Spa, Italy
#       Centre National de la Recherche Scientifique (CNRS), France
#       CS Systeme d'Information (CSSI), France
#       Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
#       Universiteit van Amsterdam (UvA), Netherlands
#       University of Helsinki (UH.HIP), Finland
#       University of Bergen (UiB), Norway
#       Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom
#
#       Authors: Elisabetta Ronchieri <elisabetta.ronchieri@cnaf.infn.it>
#       Version info: $Id$
#       Release: $Name$
#
#       Revision history:
#       $Log$
#       Revision 1.13  2012/09/07 11:50:28  mcecchi
#       moved from configuration
#
#       Revision 1.12  2007/11/22 22:40:03  giaco
#       - general cleanup of the build system
#       - now a single library libglite_wms_common instead of N, as it was before.
#         The single lib contains only utilities, configuration and logger. All
#         the other functionality will be embedded in the component that makes
#         direct use of it
#
#       Revision 1.11  2005/06/10 15:07:21  giaco
#       - exclude test from SUBDIRS, it doesn't build
#
#       Revision 1.10  2004/10/11 15:12:36  eronchie
#       Added LICENSE handling
#
#       Revision 1.9  2004/07/23 07:47:55  eronchie
#       Removed test in SUBDIRS
#
#       Revision 1.8  2004/06/18 12:12:42  eronchie
#       Start handling interface
#
#       Revision 1.7  2004/06/16 15:51:36  eronchie
#       Added configure options
#       WARNING: work in progress
#
#       Revision 1.6  2004/06/15 23:07:26  dimeglio
#       Added licence boilerplate
#
#

## Process this file with automake to produce Makefile.in

EXTRA_DIST = LICENSE

docdir = $(datadir)/doc/@PACKAGE@-@VERSION@
doc_DATA = LICENSE

## Subdirectories list
SUBDIRS = src interface config

## Default flags to run aclocal
ACLOCAL_AMFLAGS = -I project

stage:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	dot_seen=no; \
        target="install"; \
	prefix_arg=@prefix@; \
        list='$(SUBDIRS)'; for subdir in $$list; do \
          echo "Making $$target $$prefix_arg in $$subdir"; \
          if test "$$subdir" = "."; then \
            dot_seen=yes; \
            local_target="$$target-am"; \
          else \
            local_target="$$target"; \
          fi; \
          (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target $$prefix_arg) \
           || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
        done; \
        if test "$$dot_seen" = "no"; then \
          $(MAKE) $(AM_MAKEFLAGS) "$$target-am" $$prefix_arg || exit 1; \
        fi; test -z "$$fail"

distsrc: dist
	mv $(distdir).tar.gz $(DISTTAR)/$(distdir)_src.tar.gz

distbin:
	@set fnord $(MAKEFLAGS); amf=$$2; \
        dot_seen=no; \
        target="install"; \
	tmpdistbin="$(WORKDIR)/dist_bin"; \
        prefix_arg="prefix=$$tmpdistbin"; \
        list='$(SUBDIRS)'; for subdir in $$list; do \
          echo "Making $$target $$prefix_arg in $$subdir"; \
          if test "$$subdir" = "."; then \
            dot_seen=yes; \
            local_target="$$target-am"; \
          else \
            local_target="$$target"; \
          fi; \
          (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target $$prefix_arg) \
           || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
        done; \
        if test "$$dot_seen" = "no"; then \
          $(MAKE) $(AM_MAKEFLAGS) "$$target-am" $$prefix_arg || exit 1; \
        fi; test -z "$$fail"; \
	pushd $$tmpdistbin; \
	$(AMTAR) -zcf $(DISTTAR)/$(distdir)_bin.tar.gz .; \
	popd; \
	rm -rf $$tmpdistbin

