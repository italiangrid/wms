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

lib_LTLIBRARIES = \
	libglite_wms_wmproxy_security.la

noinst_HEADERS = \
	authorizer.h \
        delegation.h \
	argus_authz.h \
	vomsauthn.h

libglite_wms_wmproxy_security_la_SOURCES = \
	authorizer.cpp \
        delegation.cpp \
	argus_authz.cpp \
	gaclmanager.cpp \
	vomsauthn.cpp

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	$(BOOST_CFLAGS) \
	$(VOMS_CFLAGS) \
	$(SEC_LCMAPS_WITHOUT_GSI_CFLAGS) \
	$(SEC_LCMAPS_INTERFACE_CFLAGS) \
	$(ARGUS_PEP_API_C_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in
