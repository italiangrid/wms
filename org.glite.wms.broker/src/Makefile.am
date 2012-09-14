## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

XERCES_DIR = $(top_srcdir)/../repository/xerces-c/2.5/rhel30_gcc32
XERCESC_CFLAGS=-I$(XERCES_DIR)/include
XERCESC_LIBS=-L$(XERCES_DIR)/lib

lib_LTLIBRARIES = \
        libglite_wms_broker.la

libglite_wms_broker_la_SOURCES = \
	fuzzy_selector.cpp \
	max_selector.cpp \
	match_utils.cpp \
        match.cpp \
        exceptions.cpp \
        storage_utils.cpp \
        broker_helper.cpp \
	JobAdapter.cpp \
	JobWrapper.cpp \
	url.cpp \
	url.h \
	resolver.cpp \
        Helper_exceptions.cpp

#         brokerinfo.cpp \
#         pep_attribute.cpp \
#         pep_exceptions.cpp \
#         pep_request.cpp \
#         pep_xacml_request.cpp \
#         pep_xacml_response.cpp \
#         pep_connection.cpp \
#         pep_obligation.cpp \
#         pep_response.cpp \
#         gpbox_utils.cpp \
#

libglite_wms_broker_la_LIBADD = \
        $(GLITE_WMS_RLS_LIBS) \
        $(GLITE_WMS_OPTIONAL_GPBOX_LIBS) \
        $(GLITE_SECURITY_VOMS_CPP_THR_LIBS) \
        $(GLITE_WMS_ISM_LIBS)

#        $(XERCESC_LIBS) \
#

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/interface \
	$(GLITE_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(GLITE_WMS_OPTIONAL_GPBOX_CFLAGS) \
	$(GLITE_WMS_COMMON_SYSLOG_CFLAGS) \
	-DGLITE_WMS_DONT_HAVE_GPBOX

#  $(XERCESC_CFLAGS) \
#

MAINTAINERCLEANFILES = Makefile.in
