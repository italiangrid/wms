## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

CONF = $(top_builddir)/src/configuration/libglite_wms_conf.la

lib_LTLIBRARIES    = \
	libglite_wms_util.la \
	libglite_wms_quota.la

sbin_PROGRAMS      = glite-wms-quota-adjust

libexec_PROGRAMS   = glite-wms-eval_ad_expr 

glite_wms_eval_ad_expr_SOURCES = glite-wms-eval_ad_expr.cpp
glite_wms_eval_ad_expr_LDADD   = \
        ./libglite_wms_util.la \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS) \
        $(GLITE_JOBID_LIBS) 

glite_wms_quota_adjust_SOURCES = glite_wms_quota_adjust.cpp
glite_wms_quota_adjust_LDADD = \
        $(CONF) \
        ./libglite_wms_util.la \
        $(BOOST_FS_LIBS) -lpthread \
	$(BOOST_LIBS) \
        $(BOOST_REGEX_LIBS) \
	$(CLASSAD_LIBS) \
        $(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_JOBID_LIBS)

libglite_wms_util_la_SOURCES = \
	LineParser.cpp \
	LineParserExceptions.cpp \
	mixed.cpp \
	boost_fs_add.cpp \
	wm_commands.cpp \
	jobdir.cpp \
	input_reader.cpp \
	jobdir_reader.cpp \
        manipulation.cpp

libglite_wms_util_la_LIBADD = -lstdc++ \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_JOBID_LIBS)

libglite_wms_quota_la_SOURCES = \
	quota.cpp

utilitiesincludedir = $(includedir)/glite/wms/common/utilities
utilitiesinclude_HEADERS = \
	boost_fs_add.h \
	streamdescriptor.h \
	LineParserExceptions.h \
	quota.h \
	triple.h \
        manipulation.h

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(GLITE_WMSUTILS_CLASSADS_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
        $(LDAP_CPPFLAGS) \
	-DB_THREAD_SAFE

EXTRA_DIST = \
	triple.h 

MAINTAINERCLEANFILES = Makefile.in *.*~
