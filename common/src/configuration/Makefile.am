## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES = libglite_wms_conf.la \
	libglite_wms_conf_wrapper.la

libglite_wms_conf_la_SOURCES = \
	Configuration.cpp \
	JCConfiguration.cpp \
	NSConfiguration.cpp \
	LMConfiguration.cpp \
	ICEConfiguration.cpp \
	WMConfiguration.cpp \
	WMCConfiguration.cpp \
        WMPConfiguration.cpp \
	CommonConfiguration.cpp \
	confbase.cpp \
	exceptions.cpp \
	ModuleType.cpp

libglite_wms_conf_la_LIBADD = $(GLITE_WMSUTILS_CLASSADS_LIBS) \
			        $(BOOST_LIBS) \
			        $(CLASSAD_LIBS)

libglite_wms_conf_wrapper_la_SOURCES = \
	initializer.cpp \
	cwrapper.cpp

libglite_wms_conf_wrapper_la_LIBADD = $(GLITE_WMSUTILS_CLASSADS_LIBS) \
                                $(BOOST_LIBS) \
                                $(CLASSAD_LIBS)

configurationincludedir = $(includedir)/glite/wms/common/configuration
configurationinclude_HEADERS = \
	CommonConfiguration.h \
	cwrapper.h \
	JCConfiguration.h \
	LMConfiguration.h \
	ICEConfiguration.h \
	NSConfiguration.h \
	WMConfiguration.h \
	WMCConfiguration.h \
        WMPConfiguration.h

EXTRA_DIST = JCConfiguration.h.G \
	LMConfiguration.h.G \
	ICEConfiguration.h.G \
	NSConfiguration.h.G \
	WMConfiguration.h.G \
	WMCConfiguration.h.G \
        WMPConfiguration.h.G \
	CommonConfiguration.h.G \
	cwrapper.h.G \
	cwrapper.cpp.G \
	Configuration.def

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(GLITE_WMSUTILS_CLASSADS_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS)

CLEANFILES = $(srcdir)/JCConfiguration.h \
        $(srcdir)/LMConfiguration.h \
	$(srcdir)/ICEConfiguration.h \
        $(srcdir)/NSConfiguration.h \
        $(srcdir)/WMConfiguration.h \
        $(srcdir)/WMCConfiguration.h \
        $(srcdir)/WMPConfiguration.h \
        $(srcdir)/CommonConfiguration.h \
        $(srcdir)/cwrapper.{cpp,h}

MAINTAINERCLEANFILES = \
	Makefile.in \
	*~

GENERATOR_DEFINITION_FILE = $(srcdir)/Configuration.def

$(top_srcdir)/src/configuration/Configuration.cpp:	JCConfiguration.h LMConfiguration.h ICEConfiguration.h NSConfiguration.h WMConfiguration.h WMCConfiguration.h WMPConfiguration.h CommonConfiguration.h
$(top_srcdir)/src/configuration/initializer.cpp:	cwrapper.h cwrapper.cpp

include $(top_srcdir)/src/scripts/Makefile.gen
