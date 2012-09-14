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

lib_LTLIBRARIES = libglite_wms_wmproxy_utilities.la

noinst_HEADERS = \
	logging.h \
	wmpexception_codes.h \
	wmpexceptions.h \
	utils.h

libglite_wms_wmproxy_utilities_la_SOURCES = \
	wmpexceptions.cpp \
	utils.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src/ \
	$(CLASSAD_CFLAGS) \
	$(GLITE_WMSUTILS_EXCEPTION_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(LIBTAR_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in


