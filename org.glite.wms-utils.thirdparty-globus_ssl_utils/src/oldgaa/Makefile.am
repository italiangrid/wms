## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

noinst_LTLIBRARIES = libglobus_oldgaa.la

libglobus_oldgaa_la_SOURCES = \
	oldgaa_alloc.c \
	oldgaa_api.c \
	oldgaa_gl_internal_err.c \
	oldgaa_gl_internal_err.h \
	oldgaa_policy_evaluator.c \
	oldgaa_policy_evaluator.h \
	oldgaa_release.c \
	oldgaa_utils.c \
	oldgaa_utils.h \
	globus_oldgaa.h \
	globus_oldgaa_utils.c \
	globus_oldgaa_utils.h \
	rfc1779.c

EXTRA_DIST = globus_config.h \
	globus_oldgaa.h \
	globus_oldgaa_utils.h \
	oldgaa_gl_internal_err.h \
	oldgaa_policy_evaluator.h \
	oldgaa_utils.h

AM_CPPFLAGS = $(GLOBUS_THR_CFLAGS) 

MAINTAINERCLEANFILES = Makefile.in
