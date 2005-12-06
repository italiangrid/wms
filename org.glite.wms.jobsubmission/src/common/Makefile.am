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
#       Revision 1.9  2005/12/06 12:53:28  gianelle
#       Change the name of some "ifdef" parameters:
#       ENABLE_LBPROXY --> GLITE_WMS_HAVE_LBPROXY
#       ENABLE_LOGGING --> GLITE_WMS_HAVE_LOGGING
#       ENABLE_RENEWAL --> GLITE_WMS_HAVE_RENEWAL
#       ENABLE_PURGER  --> GLITE_WMS_HAVE_PURGER
#
#       Revision 1.8  2005/09/20 14:11:37  gianelle
#       Syncronize the rc1 branch with head.
#
#       Revision 1.7  2004/08/10 12:01:43  eronchie
#       Updated
#
#       Revision 1.6  2004/07/30 08:15:40  eronchie
#       purger.h is not located in interface but in src
#
#       Revision 1.5  2004/07/28 14:38:58  pandreet
#       Proxyrenewal dependencies fixed
#
#       Revision 1.4  2004/07/28 13:51:38  pandreet
#       Purger dependencies fixed
#
#       Revision 1.3  2004/07/27 13:46:37  pandreet
#       LB dependencies fixed
#
#       Revision 1.2  2004/07/18 11:53:35  eronchie
#       Added AC_GLITE_LB check
#       WARNING: work in progress
#
#       Revision 1.1  2004/07/06 10:05:38  pandreet
#       Basic build files for jobsubmission
#

lib_LTLIBRARIES = libglite_wms_jss_common.la

noinst_HEADERS = \
	constants.h \
	files.h \
	EventLogger.h \
	PointerId.h \
	IdCompare.h \
	IdContainer.h \
	RamContainer.h \
	JobFilePurger.h \
	ProxyUnregistrar.h \
	LockFile.h \
	SignalChecker.h \
	handler.h

libglite_wms_jss_common_la_SOURCES = IdContainer.cpp \
	RamContainer.cpp \
	PointerId.cpp \
	EventLogger.cpp \
	JobFilePurger.cpp \
	ProxyUnregistrar.cpp \
	constants.cpp \
	files.cpp \
	LockFile.cpp \
	SignalChecker.cpp \
	handler.c

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS) \
	-I$(top_srcdir)/../org.glite.wms.purger/src \
	-DGLITE_WMS_HAVE_LOGGING -DGLITE_WMS_HAVE_PURGER \
	-DGLITE_WMS_HAVE_RENEWAL -DGLITE_WMS_HAVE_LBPROXY

MAINTAINERCLEANFILES = Makefile.in *~
