## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

CONF = $(top_builddir)/src/configuration/libglite_wms_conf.la

sbin_PROGRAMS      = glite-wms-quota-adjust glite-wms-fl2jd

lib_LTLIBRARIES    = \
	libglite_wms_util.la \
	libglite_wms_globus_ftp_util.la \
	libglite_wms_iiattrutil.la \
	libglite_wms_quota.la

libexec_PROGRAMS   = glite-wms-eval_ad_expr

glite_wms_fl2jd_SOURCES = glite-wms-fl2jd.cpp
glite_wms_fl2jd_LDADD = \
	libglite_wms_util.la \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

glite_wms_eval_ad_expr_SOURCES = glite-wms-eval_ad_expr.cpp
glite_wms_eval_ad_expr_LDADD   = \
        ./libglite_wms_util.la \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

glite_wms_quota_adjust_SOURCES = glite_wms_quota_adjust.cpp
glite_wms_quota_adjust_LDADD = \
        $(CONF) \
        ./libglite_wms_util.la \
        $(BOOST_FS_LIBS) -lpthread \
        $(BOOST_REGEX_LIBS) \
	$(CLASSAD_LIBS)
glite_wms_quota_adjust_LDFLAGS = -all-static

libglite_wms_globus_ftp_util_la_SOURCES = \
	globus_ftp_mkdir.cpp \
	globus_ftp_put.cpp \
        globus_ftp_get.cpp \
	globus_ftp_exists.cpp

libglite_wms_util_la_SOURCES = \
	filecontainer.cpp \
	fstreamlock.cpp \
	FileList.cpp \
	FileListIterator.cpp \
	FileListLock.cpp \
	FileLocker.cpp \
	LineParser.cpp \
	LineParserExceptions.cpp \
	mixed.cpp \
	boost_fs_add.cpp \
	wm_commands.cpp \
	jobdir.cpp \
	input_reader.cpp \
	jobdir_reader.cpp \
	filelist_reader.cpp

libglite_wms_util_la_LIBADD = -lstdc++ \
	$(BOOST_DATE_LIBS)

libglite_wms_iiattrutil_la_SOURCES = \
	ii_attr_utils.cpp

libglite_wms_quota_la_SOURCES = \
	quota.cpp

utilitiesincludedir = $(includedir)/glite/wms/common/utilities
utilitiesinclude_HEADERS = \
	boost_fs_add.h \
	streamdescriptor.h \
	globus_ftp_macros.h \
	globus_ftp_utils.h \
	ii_attr_utils.h \
	LineParserExceptions.h \
	quota.h \
	triple.h

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
        $(LDAP_CPPFLAGS) \
	-DB_THREAD_SAFE

EXTRA_DIST = \
	triple.h 

MAINTAINERCLEANFILES = Makefile.in *.*~
