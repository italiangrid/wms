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
#       Revision 1.2.28.2.6.2.2.4  2012/06/20 13:44:33  pandreet
#       Used pkgconfig macro for condorg
#
#       Revision 1.2.28.2.6.2.2.3  2012/06/18 11:58:06  mcecchi
#       reintroduced id repository in JC. cleanup
#
#       Revision 1.2.28.2.6.2.2.2  2012/02/10 16:17:07  mcecchi
#       filelist in isolated JC/LM locations. possible future redesign
#
#       Revision 1.2.28.2.6.2.2.1  2012/02/07 14:36:30  mcecchi
#       removed support for filelist - work in progress
#
#       Revision 1.2.28.2.6.2  2011/03/11 15:00:04  pandreet
#       Removed with-glite-location
#
#       Revision 1.2.28.2.6.1  2011/01/05 08:50:58  pandreet
#       New build definitions for jobsubmission
#
#       Revision 1.2.28.2  2007/01/22 15:46:05  gianelle
#       Add support for JobDir (for the moment only the read part)
#
#       Revision 1.2.28.1  2006/10/10 11:33:15  gianelle
#       Install SubmitAdapter.h because it is used by the planner
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
	JobControllerClientJD.cpp


libglite_wms_jss_controller_la_LIBADD = ../common/libglite_wms_jss_common.la \
					$(CONDORG_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/src \
	-I$(top_srcdir)/interface \
	-I/usr/include \
	$(GLITE_WMS_COMMON_CONF_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(CONDORG_CFLAGS) -DCONDORG_VERSION=$(CONDORG_VERSION)

MAINTAINERCLEANFILES = Makefile.in *~
