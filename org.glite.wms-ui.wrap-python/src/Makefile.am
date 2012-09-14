# *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

EXTRA_DIST = AdWrapper_wrap.cpp \
	LbWrapper_wrap.cpp \
	LogWrapper_wrap.cpp \
	UcWrapper_wrap.cpp \
	SdWrapper_wrap.cpp \
	glite_wmsui_AdWrapper.py \
	glite_wmsui_UcWrapper.py \
	glite_wmsui_SdWrapper.py \
	glite_wmsui_LbWrapper.py \
	glite_wmsui_LogWrapper.py \
	glite_wmsui_AdWrapper.pyc \
	glite_wmsui_UcWrapper.pyc \
	glite_wmsui_SdWrapper.pyc \
	glite_wmsui_LbWrapper.pyc \
	glite_wmsui_LogWrapper.pyc

lib_LTLIBRARIES = \
	libglite_wmsui_LbWrapper.la \
	libglite_wmsui_LogWrapper.la \
	libglite_wmsui_UcWrapper.la \
	libglite_wmsui_SdWrapper.la \
	libglite_wmsui_AdWrapper.la

uicommonlibdir = $(libdir)/python
uicommonlib_SCRIPTS=glite_wmsui_AdWrapper.py \
	glite_wmsui_UcWrapper.py \
	glite_wmsui_SdWrapper.py \
	glite_wmsui_LbWrapper.py \
	glite_wmsui_LogWrapper.py \
	glite_wmsui_AdWrapper.pyc \
	glite_wmsui_UcWrapper.pyc \
	glite_wmsui_SdWrapper.pyc \
	glite_wmsui_LbWrapper.pyc \
	glite_wmsui_LogWrapper.pyc

noinst_HEADERS = \
	AdWrapper.h \
	LbWrapper.h \
	LogWrapper.h \
	UcWrapper.h \
	SdWrapper.h


install-exec-hook:
	cd $(DESTDIR)$(libdir); \
	for file in $(WMS_UI_COM_GLITE); do \
	ln -fs lib$$file _$$file; \
	done




libglite_wmsui_LbWrapper_la_SOURCES = \
	LbWrapper_wrap.cpp \
	LbWrapper.cpp \
	glite_wmsui_LbWrapper.i
libglite_wmsui_LbWrapper_la_LDFLAGS = \
	-rpath @prefix@
libglite_wmsui_LbWrapper_la_LIBADD = \
	-lc \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
	$(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(VOMS_CPP_NOTHR_LIBS) \
	$(GLITE_LB_NOTHR_CLIENT_LIBS) \
	$(GLITE_LB_NOTHR_CLIENTPP_LIBS) \
	$(GLITE_LB_NOTHR_COMMON_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS) \
        $(EXPAT_LIBS) \
	-lstdc++

libglite_wmsui_LogWrapper_la_SOURCES = \
	LogWrapper_wrap.cpp \
	LogWrapper.cpp \
	glite_wmsui_LogWrapper.i
libglite_wmsui_LogWrapper_la_LDFLAGS = \
	-rpath @prefix@
libglite_wmsui_LogWrapper_la_LIBADD = \
	-lc \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
        $(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(VOMS_CPP_NOTHR_LIBS) \
	$(GLITE_LB_NOTHR_CLIENT_LIBS) \
        $(GLITE_LB_NOTHR_CLIENTPP_LIBS) \
        $(GLITE_LB_NOTHR_COMMON_LIBS) \
	$(CLASSAD_LIBS) \
        $(BOOST_LIBS) \
        $(EXPAT_LIBS) \
	-lstdc++

libglite_wmsui_UcWrapper_la_SOURCES = \
	UcWrapper_wrap.cpp \
	UcWrapper.cpp \
	glite_wmsui_UcWrapper.i
libglite_wmsui_UcWrapper_la_LDFLAGS = \
	-rpath @prefix@
libglite_wmsui_UcWrapper_la_LIBADD = \
	-lc \
	$(VOMS_CPP_NOTHR_LIBS) \
	-lstdc++



libglite_wmsui_SdWrapper_la_SOURCES = \
	SdWrapper_wrap.cpp \
	SdWrapper.cpp \
	glite_wmsui_SdWrapper.i
libglite_wmsui_SdWrapper_la_LDFLAGS = \
	-rpath @prefix@
libglite_wmsui_SdWrapper_la_LIBADD = \
	-lc \
	$(VOMS_CPP_NOTHR_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS) \
	$(GLITE_SERVICE_DISCOVERY_API_C_LIBS) \
	-lstdc++




libglite_wmsui_AdWrapper_la_SOURCES = \
	AdWrapper_wrap.cpp \
	AdWrapper.cpp \
	glite_wmsui_AdWrapper.i
libglite_wmsui_AdWrapper_la_LDFLAGS = \
	-rpath @prefix@
libglite_wmsui_AdWrapper_la_LIBADD = \
	-lc \
	$(GLITE_WMSUTILS_CJOBID_LIBS) \
        $(GLITE_WMSUTILS_JOBID_LIBS) \
	$(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(GLITE_JDL_LIBS) \
	$(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
	$(GLITE_LB_NOTHR_CLIENT_LIBS) \
	$(GLITE_LB_NOTHR_CLIENTPP_LIBS) \
	$(GLITE_LB_NOTHR_COMMON_LIBS) \
	$(CLASSAD_LIBS) \
        $(BOOST_LIBS) \
        $(EXPAT_LIBS) \
	-lstdc++

LbWrapper_wrap.cpp : glite_wmsui_LbWrapper.i
	@#Create the files
	@echo "Swig compiling file $< ...."
	$(SWIG_BIN_PATH)/swig -I$(SWIG_INSTALL_PATH)/lib/swig1.3 -I$(SWIG_INSTALL_PATH)/lib/swig1.3/python -python -shadow -c++ -o $@ $<

LogWrapper_wrap.cpp : glite_wmsui_LogWrapper.i
	@#Create the files
	@echo "Swig compiling file $< ...."
	$(SWIG_BIN_PATH)/swig -I$(SWIG_INSTALL_PATH)/lib/swig1.3 -I$(SWIG_INSTALL_PATH)/lib/swig1.3/python -python -shadow -c++ -o $@ $<

UcWrapper_wrap.cpp: glite_wmsui_UcWrapper.i
	@#Create the files
	@echo "Swig compiling file .CPP and .PY $< ...."
	$(SWIG_BIN_PATH)/swig -I$(SWIG_INSTALL_PATH)/lib/swig1.3 -I$(SWIG_INSTALL_PATH)/lib/swig1.3/python -python -shadow -c++ -o $@ $<

SdWrapper_wrap.cpp: glite_wmsui_SdWrapper.i
	@#Create the files
	@echo "Swig compiling file .CPP and .PY $< ...."
	$(SWIG_BIN_PATH)/swig -I$(SWIG_INSTALL_PATH)/lib/swig1.3 -I$(SWIG_INSTALL_PATH)/lib/swig1.3/python -python -shadow -c++ -o $@ $<

AdWrapper_wrap.cpp : glite_wmsui_AdWrapper.i
	@#Create the files
	@echo "Swig compiling file $< ...."
	$(SWIG_BIN_PATH)/swig -I$(SWIG_INSTALL_PATH)/lib/swig1.3 -I$(SWIG_INSTALL_PATH)/lib/swig1.3/python -python -shadow -c++ -o $@ $<

glite_wmsui_AdWrapper.pyc:
	/bin/touch glite_wmsui_AdWrapper.pyc

glite_wmsui_UcWrapper.pyc:
	/bin/touch glite_wmsui_UcWrapper.pyc

glite_wmsui_SdWrapper.pyc:
	/bin/touch glite_wmsui_SdWrapper.pyc

glite_wmsui_LbWrapper.pyc:
	/bin/touch glite_wmsui_LbWrapper.pyc

glite_wmsui_LogWrapper.pyc:
	/bin/touch glite_wmsui_LogWrapper.pyc

glite_wmsui_AdWrapper.py:
	/bin/touch glite_wmsui_AdWrapper.py

glite_wmsui_UcWrapper.py:
	/bin/touch glite_wmsui_UcWrapper.py

glite_wmsui_SdWrapper.py:
	/bin/touch glite_wmsui_SdWrapper.py

glite_wmsui_LbWrapper.py:
	/bin/touch glite_wmsui_LbWrapper.py

glite_wmsui_LogWrapper.py:
	/bin/touch glite_wmsui_LogWrapper.py

WMS_UI_COM_GLITE = glite_wmsui_AdWrapper.a \
	glite_wmsui_AdWrapper.la \
	glite_wmsui_AdWrapper.so \
	glite_wmsui_AdWrapper.so.0 \
	glite_wmsui_AdWrapper.so.0.0.0 \
	glite_wmsui_LbWrapper.a \
	glite_wmsui_LbWrapper.la \
	glite_wmsui_LbWrapper.so \
	glite_wmsui_LbWrapper.so.0 \
	glite_wmsui_LbWrapper.so.0.0.0 \
	glite_wmsui_LogWrapper.a \
	glite_wmsui_LogWrapper.la \
	glite_wmsui_LogWrapper.so \
	glite_wmsui_LogWrapper.so.0 \
	glite_wmsui_LogWrapper.so.0.0.0 \
	glite_wmsui_UcWrapper.a \
	glite_wmsui_UcWrapper.la \
	glite_wmsui_UcWrapper.so \
	glite_wmsui_UcWrapper.so.0 \
	glite_wmsui_UcWrapper.so.0.0.0 \
	glite_wmsui_SdWrapper.a \
	glite_wmsui_SdWrapper.la \
	glite_wmsui_SdWrapper.so \
	glite_wmsui_SdWrapper.so.0 \
	glite_wmsui_SdWrapper.so.0.0.0

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(PYTHON_CFLAGS) \
	$(GLOBUS_NOTHR_CFLAGS) \
	$(GLOBUS_SSL_UTILS_CFLAGS) \
	$(EXPAT_CFLAGS) \
	-DWANT_NAMESPACES

CLEANFILES = AdWrapper_wrap.cpp \
	LbWrapper_wrap.cpp \
	LogWrapper_wrap.cpp \
	UcWrapper_wrap.cpp \
	SdWrapper_wrap.cpp \
	glite_wmsui_AdWrapper.py \
	glite_wmsui_UcWrapper.py \
	glite_wmsui_SdWrapper.py \
	glite_wmsui_LbWrapper.py \
	glite_wmsui_LogWrapper.py \
	glite_wmsui_AdWrapper.pyc \
	glite_wmsui_UcWrapper.pyc \
	glite_wmsui_SdWrapper.pyc \
	glite_wmsui_LbWrapper.pyc \
	glite_wmsui_LogWrapper.pyc

MAINTAINERCLEANFILES = Makefile.in
