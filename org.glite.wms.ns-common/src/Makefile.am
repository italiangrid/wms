lib_LTLIBRARIES = \
	libnsclientcommands.la \
	libnsservercommands.la \
	lib_glite_pipe.la

nscommondir = $(includedir)/glite/wms/ns-common

nscommon_HEADERS = \
	versions/client_ver.h \
	versions/server_ver.h \
	versions/Version.h \
        CommandState.h \
        fsm/ExecuteFunction.h \
        fsm/ForwardRequest.h \
        fsm/ReceiveBoolean.h \
        fsm/ReceiveInt.h \
        fsm/ReceiveLong.h \
        fsm/ReceiveString.h \
        fsm/ReceiveVector.h \
        fsm/SendBoolean.h \
        fsm/SendInt.h \
        fsm/SendJDL.h \
        fsm/SendLong.h \
        fsm/SendString.h \
        fsm/SendVector.h \
        fsm/Jump.h \
        fsm/Label.h \
	commands/exception_codes.h \
	commands/NetworkServer.h \
	commands/const.h \
	commands/JobId.h \
	commands/DagCommandFactoryClientImpl.h \
	commands/DagCommandFactoryServerImpl.h \
	commands/CommandFactoryClientImpl.h \
	commands/CommandFactory.h \
	commands/CommandFactoryImpl.h \
	commands/CommandFactoryServerImpl.h \
	commands/Command.h \
	commands/common_dag.h \
	commands/common.h \
	commands/listjobmatch.h \
	commands/gsimkdirex.h \
	commands/listfiles.h \
	commands/logging.h \
	commands/MatchingPipe.h \
	commands/MatchingPipe_nb.h \
	commands/MatchingPipeFiller.h

libnsclientcommands_la_SOURCES = \
	fsm/ExecuteFunction.cpp \
        fsm/ForwardRequest.cpp \
        fsm/ReceiveBoolean.cpp \
        fsm/ReceiveLong.cpp \
        fsm/ReceiveString.cpp \
        fsm/ReceiveVector.cpp \
        fsm/ReceiveInt.cpp \
        fsm/SendBoolean.cpp \
        fsm/SendLong.cpp \
        fsm/SendString.cpp \
        fsm/SendVector.cpp \
        fsm/SendInt.cpp \
        fsm/Jump.cpp \
        fsm/Label.cpp \
	commands/gsimkdirex.cpp \
	commands/common.cpp \
	commands/Command.cpp \
	commands/CommandFactoryClientImpl.cpp \
	commands/DagCommandFactoryClientImpl.cpp

libnsservercommands_la_SOURCES = \
	versions/Version.cpp \
	fsm/ExecuteFunction.cpp \
        fsm/ForwardRequest.cpp \
        fsm/ReceiveBoolean.cpp \
        fsm/ReceiveLong.cpp \
        fsm/ReceiveString.cpp \
        fsm/ReceiveVector.cpp \
        fsm/ReceiveInt.cpp \
        fsm/SendBoolean.cpp \
        fsm/SendLong.cpp \
        fsm/SendString.cpp \
        fsm/SendVector.cpp \
        fsm/SendInt.cpp \
        fsm/Jump.cpp \
        fsm/Label.cpp \
	commands/common.cpp \
	commands/Command.cpp \
	commands/listjobmatch.cpp \
	commands/listfiles.cpp \
	commands/CommandFactoryServerImpl.cpp \
	commands/DagCommandFactoryServerImpl.cpp

lib_glite_pipe_la_SOURCES = \
	commands/MatchingPipe.cpp \
	commands/MatchingPipe_nb.cpp \
	commands/MatchingPipeFiller.cpp

libnsservercommands_la_LIBADD = \
        $(GLITE_WMS_PURGER_LIBS)

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/commands \
	-I$(top_srcdir)/src/fsm \
	$(GLITE_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(GLOBUS_THR_CFLAGS) \
	-DSHOW_EXECUTION \
	-DWITH_GLOBUS_FTP_CLIENT_API \
        -DGLITE_WMS_HAVE_LBPROXY

MAINTAINERCLEANFILES = Makefile.in









