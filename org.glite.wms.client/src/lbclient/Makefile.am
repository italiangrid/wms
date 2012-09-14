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
#       Authors: Giuseppe Avellino <giuseppe.avellino@datamat.it>
#
# WARNING: work in progress


envC.cpp : env.h
	@#Create the files
	@echo "gsoap compiling .h"
	$(GSOAP_LOCATION)/soapcpp2 -I$(GSOAP_LOCATION) -penv $(top_srcdir)/src/lbclient/env.h

lb.h :
	@#Create the files
	@echo "wsdl2h parsing .wsdl"
	$(GSOAP_LOCATION)/wsdl2h -o lb.h \
	-t$(GSOAP_LOCATION)/typemap.dat \
	$(GLITE_LOCATION)/interface/LB.wsdl

lbClientLib.cpp : lb.h
	@#Create the files
	@echo "gsoap compiling .h"
	$(GSOAP_LOCATION)/soapcpp2 -I$(GSOAP_LOCATION) -n -plb $(top_srcdir)/build/src/lbclient/lb.h



lib_LTLIBRARIES = libglite_wms_client_lb.la

libglite_wms_client_lb_la_SOURCES = \
	lb.h \
	envC.cpp \
	lbClientLib.cpp \
	stdsoap2.cpp


AM_CPPFLAGS = -I$(top_srcdir)/src \
	-I$(GSOAP_LOCATION) \
	$(GLITE_CFLAGS) \
	-DWITH_OPENSSL -DWITH_NONAMESPACES


MAINTAINERCLEANFILES = Makefile.in

