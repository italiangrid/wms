##
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

lib_LTLIBRARIES = \
	libglite_wms_wmproxy_eventlogger.la


noinst_HEADERS = \
	expdagad.h \
	eventlogger.h

libglite_wms_wmproxy_eventlogger_la_SOURCES = \
	expdagad.cpp \
	eventlogger.cpp

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	$(GLITE_JDL_CFLAGS) \
	$(GLITE_WMSUTILS_EXCEPTION_CFLAGS) \
	$(GLITE_WMS_COMMON_CONF_CFLAGS) \
	$(GLITE_WMS_COMMON_LOGGER_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS) 
	
MAINTAINERCLEANFILES = Makefile.in
