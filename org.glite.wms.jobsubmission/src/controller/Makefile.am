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
#       Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
#       Version info: $Id$
#       Release: $Name$
#
#       Revision history:
#       $Log$
#       Revision 1.5  2009/02/05 16:07:23  mcecchi
#       enabling bunch submission to condor (work in progress)
#
#       Revision 1.4  2008/10/01 13:13:06  mcecchi
#       enabling multiple job submission for a single user
#
#       Revision 1.3  2007/01/22 15:47:19  gianelle
#       Sincronize HEAD repository with the 3.1 branch
#       Add the JobDir support
#
#       Revision 1.2  2004/08/10 12:01:43  eronchie
#       Updated
#
#       Revision 1.1  2004/07/06 10:05:38  pandreet
#       Basic build files for jobsubmission
#

lib_LTLIBRARIES = libglite_wms_jss_controller.la \
	libglite_wms_jss_controller_wrapper.la \
	libglite_wms_jss_controller_adapter.la

noinst_HEADERS = \
	CondorG.h \
	cwrapper.h \
	JobControllerExceptions.h \
	JobControllerFactory.h \
	JobControllerFake.h \
	JobController.h \
	JobControllerImpl.h \
	JobControllerProxy.h \
	JobControllerReal.h \
	JobControllerClient.h \
	JobControllerClientImpl.h \
	JobControllerClientReal.h \
	JobControllerClientJD.h \
	Request.h \
	RequestExceptions.h \
	SubmitAd.h \
	SubmitAdExceptions.h

libglite_wms_jss_controller_wrapper_la_SOURCES = cwrapper.cpp

libglite_wms_jss_controller_adapter_la_SOURCES = SubmitAd.cpp \
	SubmitAdExceptions.cpp \
	SubmitAdapter.cpp

libglite_wms_jss_controller_la_SOURCES = CondorG.cpp \
	Request.cpp \
	RequestExceptions.cpp \
	JobController.cpp \
	JobControllerExceptions.cpp \
	JobControllerFactory.cpp \
	JobControllerProxy.cpp \
	JobControllerReal.cpp \
	JobControllerFake.cpp \
	JobControllerClient.cpp \
	JobControllerClientJD.cpp \
	JobControllerClientReal.cpp

libglite_wms_jss_controller_la_LIBADD = $(CONDORG_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/src \
        -I$(top_srcdir)/interface \
	$(GLITE_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(CONDORG_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~
