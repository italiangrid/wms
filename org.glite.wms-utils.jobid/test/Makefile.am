## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

JOBID_LIBS = $(top_builddir)/src/jobid/libglite_wmsutils_jobid.la 
CJOBID_LIBS = $(top_builddir)/src/jobid/libglite_wmsutils_cjobid.la

TESTS = glite-wmsutils-jobid \
		glite-wmsutils-manipulation

check_PROGRAMS = $(TESTS)

glite_wmsutils_jobid_SOURCES = jobid_cu_suite.cpp \
	jobid_cu_suite.h \
	jobid_cu_main.cpp
	
glite_wmsutils_jobid_LDADD =  \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLOBUS_GSS_THR_LIBS) \
	$(CPPUNIT_LIBS) \
	$(JOBID_LIBS) \
	$(CJOBID_LIBS)
	
glite_wmsutils_manipulation_SOURCES = manipulation_cu_suite.cpp \
	manipulation_cu_suite.h \
	manipulation_cu_main.cpp
	
glite_wmsutils_manipulation_LDADD =  \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLOBUS_GSS_THR_LIBS) \
	$(CPPUNIT_LIBS) \
	$(JOBID_LIBS) \
	$(CJOBID_LIBS)


AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/test \
	$(GLITE_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	$(CPPUNIT_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~

