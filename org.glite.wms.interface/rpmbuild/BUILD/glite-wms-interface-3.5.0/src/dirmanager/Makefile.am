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
#       Authors: Giuseppe Avellino <egee@datamat.it>
#
# WARNING: work in progress


glite_wms_wmproxy_dirmanager_SOURCES = \
	glite-wms-wmproxy-dirmanager.c

libexec_PROGRAMS = glite_wms_wmproxy_dirmanager

glite_wms_wmproxy_dirmanager_LDADD = \
	$(LIBTAR_LIBS) \
	$(ZLIB_LIBS)

AM_CPPFLAGS = \
	$(LIBTAR_CFLAGS) \
        $(ZLIB_CFLAGS) \
	-DALLOW_COMMAND_LINE_OVERRIDE

MAINTAINERCLEANFILES = Makefile.in
