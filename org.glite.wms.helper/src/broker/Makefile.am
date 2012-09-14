## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wms_helper_broker_ism.la

libglite_wms_helper_broker_ism_la_SOURCES = \
	classad_plugin_loader.cpp \
        classad_plugin.cpp \
        doMatch.cpp \
        listAttrRegEx.cpp \
        measured_response_time.cpp \
        successFraction.cpp \
        testMemberEx.cpp \
        retrieveCloseSEsInfo.cpp \
        matchmaker.cpp \
	matchmakerISMImpl.cpp \
	mm_exceptions.cpp \
	datalocationinterfaceC.cpp \
	datalocationinterfaceClient.cpp \
	DataLocationInterfaceSOAP.cpp \
        storage_utils.cpp \
        dli_utils.cpp \
        brokerinfo.cpp \
        ResourceBroker.cpp \
        RBSimpleISMImpl.cpp \
        RBMaximizeFilesISMImpl.cpp \
        RBSelectionSchema.cpp \
        maxRankSelector.cpp \
        stochasticRankSelector.cpp \
        exceptions.cpp \
        Helper.h \
        Helper_ism.cpp \
        Helper_matcher_ism.cpp

libglite_wms_helper_broker_ism_la_LIBADD = \
        $(CLASSAD_LIBS) \
        $(BOOST_LIBS) \
	$(GLITE_JDL_LIBS) \
	$(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_COMMON_LOGGER_LIBS) \
	$(GLITE_WMS_MATCHMAKING_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_LB_CLIENT_LIBS) \
        $(GLITE_WMS_ISM_LIBS) \
	$(GSOAP_LIBS) \
	$(GSOAP_PLUGIN_LIBS)

datalocationinterfaceClient.cpp datalocationinterfaceC.cpp : datalocationinterface.h
	$(GSOAP_LOCATION)/bin/soapcpp2 -I$(GSOAP_LOCATION)/include -I$(GSOAP_LOCATION)/share/gsoap/import \
	-C -n -pdatalocationinterface $(top_builddir)/src/broker/datalocationinterface.h

datalocationinterface.h : $(top_srcdir)/src/broker/DataLocationInterface.wsdl
	$(GSOAP_LOCATION)/bin/wsdl2h -t $(top_srcdir)/src/broker/catalogs_typemap.dat \
	-o datalocationinterface.h $(top_srcdir)/src/broker/DataLocationInterface.wsdl

AM_CPPFLAGS = \
	-I$(top_srcdir)/interface \
	$(GLITE_JDL_CFLAGS) \
        $(GLITE_WMS_ISM_CFLAGS) \
	$(GLITE_WMS_COMMON_CONF_CFLAGS) \
	$(GLITE_WMS_COMMON_LOGGER_CFLAGS) \
	$(GLITE_WMSUTILS_CLASSADS_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in

