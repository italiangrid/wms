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
#       Revision 1.5  2006/09/19 13:44:24  monforte
#       merge against 3.1 and code review
#
#       Revision 1.4  2006/03/13 14:38:05  monforte
#       Merge against glite-wms-matchmaking_branch_1_5_0_RC15
#
#       Revision 1.3.18.1  2006/02/02 18:41:32  fprelz
#       Adding matchmaking performance patches (as tested by ECGI at end 2005 on v1.4)
#
#       Revision 1.3  2004/10/08 15:08:47  monforte
#       fixed LICENCE problem
#
#       Revision 1.2  2004/07/28 17:45:27  eronchie
#       Ping Puzzetta Puzzosa
#
#       Revision 1.1  2004/06/26 15:25:03  eronchie
#       Start handling matchmaking
#       WARNING: work in progress
#
#
#

## Process this file with automake to produce Makefile.in

EXTRA_DIST = LICENSE

docdir = $(datadir)/doc/@PACKAGE@-@VERSION@
doc_DATA = LICENSE

## Subdirectories list
SUBDIRS = interface src

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
	
