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
WMPROXY_UTILITIES = $(top_builddir)/src/utilities/libglite_wms_wmproxy_utilities.la
WMPROXY_EVENTLOGGER = $(top_builddir)/src/eventlogger/libglite_wms_wmproxy_eventlogger.la
WMPROXY_SECURITY= $(top_builddir)/src/security/libglite_wms_wmproxy_security.la
WMPROXY_COMMANDS = $(top_builddir)/src/commands/libglite_wms_wmproxy_commands.la \
$(top_builddir)/src/commands/libglite_wms_wmproxy_pipe.la

wm.h : $(top_srcdir)/src/server/WMProxy.wsdl
	@#Create the files
	@echo "wsdl2h parsing .wsdl"
	$(GSOAP_LOCATION)/bin/wsdl2h -o wm.h \
	-I$(GSOAP_LOCATION)/include \
	-t $(top_srcdir)/src/server/wmptypemap.dat \
	$(top_srcdir)/src/server/WMProxy.wsdl

soapC.cpp soapServer.cpp soapH.h soapStub.h: $(top_builddir)/src/server/wm.h
	@#Create the files
	@echo "gsoap compiling .h"
	$(GSOAP_LOCATION)/bin/soapcpp2 -I$(GSOAP_LOCATION)/include:/usr/share/gsoap/import wm.h 

JSDL_CFLAGS= -I/usr/include/libxml2

glite_wms_wmproxy_server_SOURCES = \
	wm.h \
	soapC.cpp \
	soapServer.cpp \
	wmproxy.cpp \
	wmproxyserve.cpp \
	common.cpp \
	operations.cpp \
	coreoperations.cpp \
	gsoapoperations.cpp \
	gsoapfaultmanipulator.cpp \
	configuration.cpp \
	wmp2wm.cpp \
	structconverter.cpp \
	stdsoap2.cpp \
	signalhandler.cpp

bin_PROGRAMS = glite_wms_wmproxy_server

noinst_HEADERS = \
	wmp2wm.h \
	common.h \
	wmproxyserve.h \
	configuration.h \
	gsoapfaultmanipulator.h \
	operations.h \
	coreoperations.h \
	structconverter.h \
	responsestruct.h \
	signalhandler.h
	
glite_wms_wmproxy_server_LDFLAGS = -Xlinker --export-dynamic $(XSLT_LDFLAGS)

glite_wms_wmproxy_server_LDADD = \
	$(GLITE_LB_COMMON_LIBS) \
	$(GLITE_LB_CLIENT_LIBS) \
	$(GLITE_LB_CLIENTPP_LIBS) \
	$(RENEWAL_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_JOBID_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_JDL_LIBS) \
	$(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMS_PURGER_LIBS) \
        $(GLITE_WMS_COMMON_QUOTA_LIBS) \
	$(WMPROXY_UTILITIES) \
	$(WMPROXY_EVENTLOGGER) \
	$(WMPROXY_SECURITY) \
	$(WMPROXY_COMMANDS) \
	$(CLASSAD_LIBS) \
	$(ARES_LIBS) \
	$(BOOST_LIBS) \
	$(EXPAT_LIBS) \
	$(GRIDSITE_OPENSSL_LIBS) \
	$(SEC_LCMAPS_WITHOUT_GSI_LIBS) \
	$(SEC_LCMAPS_RETURN_WITHOUT_GSI_LIBS) \
	$(VOMS_NOTHR_LIBS) \
	$(FCGI_CPP_LIBS) -lfcgi \
	$(XSLT_LIBS) \
	$(ARGUS_PEP_API_C_LIBS)

# -DWITH_FASTCGI missing in gsoap macro
AM_CPPFLAGS = -I$(top_srcdir)/src/server \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/build/src/server \
	$(GSOAP_CFLAGS) -DWITH_FASTCGI \
	$(VOMS_CFLAGS) \
	$(EXPAT_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(GRIDSITE_OPENSSL_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(FCGI_CFLAGS) \
	$(XSLT_CPPFLAGS) \
	$(JSDL_CFLAGS) \
	-DWMP_SOFTWARE_VERSION=\"@VERSION@\" \
	-DWANT_NAMESPACES

MAINTAINERCLEANFILES = Makefile.in
