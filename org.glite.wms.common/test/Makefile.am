## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

FTP_UTIL = $(top_builddir)/src/utilities/libglite_wms_globus_ftp_util.la
UTIL = $(top_builddir)/src/utilities/libglite_wms_util.la
LOGGER = $(top_builddir)/src/logger/libglite_wms_logger.la
CONF = $(top_builddir)/src/configuration/libglite_wms_conf.la
CONF_WRAPPER = $(top_builddir)/src/configuration/libglite_wms_conf_wrapper.la
PROCESS = $(top_builddir)/src/process/libglite_wms_process.la

STDCPP = -lstdc++

WL_CMN_LOGGER_TEST = logtest \
	logthread

WL_CMN_PROCESS_TEST = proctest \
	regain

WL_CMN_UTILITIES_TEST = classadclient \
	classadextractor \
        container \
	compact \
	create \
        extractor \
        lock_client \
	lock_server \
        newclient \
	newserver \
        qclient \
	qserver \
	queue \
	show

WL_CMN_UT_TEST = glite_wms_listsTest \
	glite-wms-poplist \
        glite-wms-showlistTest

WL_CMN_OTHER_TEST = config \
	cconfig \
	gsi-mkdir \
	sftpput

WL_CMN_OT_TEST = glite-wms-classadlistTest

check_PROGRAMS = $(WL_CMN_LOGGER_TEST) \
        $(WL_CMN_PROCESS_TEST) \
        $(WL_CMN_UTILITIES_TEST) \
        $(WL_CMN_OTHER_TEST)

ts_common_bindir = $(prefix)/workload/test/bin
ts_common_bin_PROGRAMS= $(WL_CMN_UT_TEST) \
        $(WL_CMN_OT_TEST)

container_SOURCES = testcontainer.cpp
container_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

compact_SOURCES = filecompacttest.cpp
compact_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

show_SOURCES = showcontainer.cpp
show_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

glite_wms_showlistTest_SOURCES = showlist.cpp
glite_wms_showlistTest_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

glite_wms_poplist_SOURCES = poplist.cpp
glite_wms_poplist_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

create_SOURCES = createlist.cpp
create_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

queue_SOURCES = testqueue.cpp
queue_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

qserver_SOURCES = queueserver.cpp queuecommon.h
qserver_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

qclient_SOURCES = queueclient.cpp queuecommon.h
qclient_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

newserver_SOURCES = newserver.cpp queuecommon.h
newserver_LDADD = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

newclient_SOURCES = newclient.cpp queuecommon.h
newclient_LDADD = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

extractor_SOURCES = extractor.cpp queuecommon.h
extractor_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

glite_wms_listsTest_SOURCES = listtest.cpp
glite_wms_listsTest_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

config_SOURCES = configure.cpp
config_LDADD   = \
	$(CONF) \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

glite_wms_classadlistTest_SOURCES = classadlist.cpp
glite_wms_classadlistTest_LDADD   = \
	$(UTIL) \
	$(CONF) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

cconfig_SOURCES = cconfigure.c
cconfig_LDADD   = \
	$(CONF_WRAPPER) \
	$(CONF) \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS) \
	$(STDCPP)

lock_client_SOURCES = listlockclient.cpp
lock_client_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

lock_server_SOURCES = listlockserver.cpp
lock_server_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

classadclient_SOURCES = classadclient.cpp queuecommon.h
classadclient_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

classadextractor_SOURCES = classadextractor.cpp queuecommon.h
classadextractor_LDADD   = \
	$(UTIL) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

logtest_SOURCES = logtest.cpp
logtest_LDADD   = \
	$(LOGGER) \
	$(BOOST_LIBS)

logthread_SOURCES = logtest_bt.cpp
logthread_LDADD   = \
	$(LOGGER) \
	$(BOOST_LIBS)

proctest_SOURCES = proctest.cpp
proctest_LDADD   = $(PROCESS)

regain_SOURCES = regain.cpp
regain_LDADD   = $(PROCESS)

gsi_mkdir_SOURCES = gsi-mkdir.cpp
gsi_mkdir_LDADD   = \
	$(LOGGER) \
	$(UTIL) \
	$(FTP_UTIL) \
	$(GLOBUS_FTP_CLIENT_NOTHR_LIBS) \
	$(BOOST_LIBS) \
        $(CLASSAD_LIBS)

sftpput_SOURCES = sftpput.cpp
sftpput_LDADD   = \
	$(FTP_UTIL) \
	$(LOGGER) \
	$(GLOBUS_FTP_CLIENT_NOTHR_LIBS) \
        $(BOOST_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(GLOBUS_THR_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~

