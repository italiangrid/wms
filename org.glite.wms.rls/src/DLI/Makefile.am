## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = \
	libglite_wms_rls_dli.la

libglite_wms_rls_dli_la_SOURCES = \
	envC.cpp\
	datalocationinterfaceClientLib.cpp\
	DLI_ReplicaService.cpp

libglite_wms_rls_dli_la_LIBADD = \
	$(SEC_GSOAP_PLUGIN_276b_NOTHR_LIBS)

envC.cpp: $(top_srcdir)/src/DLI/env.h; \
	$(SOAPCPP2) -I$(GSOAP_LOCATION) -I$(GSOAP_LOCATION)/include \
	-C -d. -penv $(top_srcdir)/src/DLI/env.h

datalocationinterfaceClientLib.cpp: datalocationinterface.h ; \
	$(SOAPCPP2) -I$(GSOAP_LOCATION) -I$(GSOAP_LOCATION)/include \
	-C -d. -n -pdatalocationinterface $(top_srcdir)/src/DLI/datalocationinterfaceNS.h

datalocationinterface.h: ; \
	$(WSDL2H) -t $(top_srcdir)/src/DLI/catalogs_typemap.dat -o ./datalocationinterface.h \
	http://cmsdoc.cern.ch/cms/grid/doc/DataLocationInterface.wsdl	


AM_CPPFLAGS = \
	-I$(top_srcdir)/interface \
        -I$(top_srcdir)/src \
	-I$(top_srcdir)/src/DLI \
        $(GLITE_CFLAGS) \
        $(BOOST_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	$(GSOAP_CFLAGS)


MAINTAINERCLEANFILES = Makefile.in *.*~
