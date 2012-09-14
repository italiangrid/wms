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
#       Revision 1.11.2.1.4.3.2.3.2.4  2012/06/18 11:58:06  mcecchi
#       reintroduced id repository in JC. cleanup
#
#       Revision 1.11.2.1.4.3.2.3.2.3  2012/02/15 14:07:48  pandreet
#       Removed header file
#
#       Revision 1.11.2.1.4.3.2.3.2.2  2012/02/10 16:17:05  mcecchi
#       filelist in isolated JC/LM locations. possible future redesign
#
#       Revision 1.11.2.1.4.3.2.3.2.1  2012/02/07 14:36:29  mcecchi
#       removed support for filelist - work in progress
#
#       Revision 1.11.2.1.4.3.2.3  2011/03/11 15:00:04  pandreet
#       Removed with-glite-location
#
#       Revision 1.11.2.1.4.3.2.2  2011/01/05 08:50:58  pandreet
#       New build definitions for jobsubmission
#
#       Revision 1.11.2.1.4.3.2.1  2010/02/08 08:58:07  mcecchi
#       fixing some nightly builds
#
#       Revision 1.11.2.1.4.3  2009/03/19 15:45:45  mcecchi
#       LB type run-time selection for purger
#
#       Revision 1.11.2.1.4.2  2009/02/17 15:35:47  mcecchi
#       LB type run-time selection
#
#       Revision 1.11.2.1.4.1  2009/02/13 13:09:26  mcecchi
#       some cleanup
#
#       Revision 1.11.2.1  2007/02/13 12:30:27  gianelle
#       - Fix for bug #23401
#       - Add support for jobdir to LM
#       - Log a cancel refused event when the cancel failed (it was a rare circumstance
#       which is not considered before)
#       - Some cleanup in the "include" instructions
#
#       Revision 1.11  2006/03/16 17:28:39  gianelle
#       Migration to "new" jdl library
#
#       Revision 1.10  2006/03/14 21:22:14  eronchie
#       Applied PURGER macro
#
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
	fstreamlock.h \
	files.h \
	file_container.h \
	file_locker.h \
	filelist.h \
	filelist_lock.h \
	filelist_reader.h \
	filelist_iterator.h \
	filelist_extractor.h \
	pointer_id.h \
	IdCompare.h \
	id_container.h \
	ram_container.h \
	EventLogger.h \
	JobFilePurger.h \
	ProxyUnregistrar.h \
	LockFile.h \
	SignalChecker.h \
	handler.h

libglite_wms_jss_common_la_SOURCES = \
	fstreamlock.cpp \
	id_container.cpp \
	file_container.cpp \
	ram_container.cpp \
	pointer_id.cpp \
	filelist.cpp \
	filelist_iterator.cpp \
	filelist_lock.cpp \
	file_locker.cpp \
	EventLogger.cpp \
	JobFilePurger.cpp \
	ProxyUnregistrar.cpp \
	constants.cpp \
	files.cpp \
	LockFile.cpp \
	SignalChecker.cpp \
	handler.c

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GLITE_WMS_COMMON_UT_UTIL_CFLAGS) \
	-I/usr/include \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~
