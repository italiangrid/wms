TESTS = glite-wms-common-ldif2classad-LDIFObject

check_PROGRAMS = glite-wms-common-ldif2classad-LDIFObject

glite_wms_common_ldif2classad_LDIFObject_SOURCES = LDIFObject_cu_suite.cpp \
        LDIFObject_cu_suite.h \
        LDIFObject_cu_main.cpp
glite_wms_common_ldif2classad_LDIFObject_LDADD = \
        $(CPPUNIT_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
        -I$(top_srcdir)/src \
        -I$(top_srcidr)/test \
        $(GLITE_CFLAGS) \
        $(CPPUNIT_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~

