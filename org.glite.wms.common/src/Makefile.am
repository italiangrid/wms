# Copyright (c) Members of the EGEE Collaboration. 2004.
# See http://www.eu-egee.org/partners/ for details on the copyright
# holders.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

libexec_PROGRAMS   = glite-wms-eval_ad_expr
lib_LTLIBRARIES = libglite_wms_common.la

glite_wms_eval_ad_expr_SOURCES = utilities/glite-wms-eval_ad_expr.cpp
glite_wms_eval_ad_expr_LDFLAGS = $(GLITE_LDFLAGS) $(CLASSAD_LDFLAGS)
glite_wms_eval_ad_expr_LDADD   = \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS)

configuration_cpp_files = \
	configuration/Configuration.cpp \
	configuration/JCConfiguration.cpp \
	configuration/NSConfiguration.cpp \
	configuration/LMConfiguration.cpp \
	configuration/ICEConfiguration.cpp \
	configuration/WMConfiguration.cpp \
	configuration/WMPConfiguration.cpp \
	configuration/CommonConfiguration.cpp \
	configuration/confbase.cpp \
	configuration/exceptions.cpp \
	configuration/ModuleType.cpp \
	configuration/initializer.cpp \
	configuration/cwrapper.cpp

configuration_cpp_g_files = $(wildcard $(srcdir)/configuration/*.cpp.G)
configuration_generated_cpp_files = $(configuration_cpp_g_files:.G=)
configuration_h_g_files = $(wildcard $(srcdir)/configuration/*.h.G)
configuration_generated_h_files = $(configuration_h_g_files:.G=)

utilities_cpp_files = \
	utilities/filecontainer.cpp \
	utilities/fstreamlock.cpp \
	utilities/FileList.cpp \
	utilities/FileListIterator.cpp \
	utilities/FileListLock.cpp \
	utilities/FileLocker.cpp \
	utilities/LineParser.cpp \
	utilities/LineParserExceptions.cpp \
	utilities/mixed.cpp \
	utilities/boost_fs_add.cpp \
	utilities/wm_commands.cpp \
	utilities/jobdir.cpp \
	utilities/input_reader.cpp \
	utilities/jobdir_reader.cpp \
	utilities/filelist_reader.cpp

logger_cpp_files = \
	logger/common.cpp \
	logger/logbuf.cpp \
	logger/logstream.cpp \
	logger/manipulators.cpp \
	logger/logstream_ts.cpp \
	logger/container_ts.cpp \
	logger/manipulators_ts.cpp \
	logger/key_value.cpp \
	logger/logging.cpp

libglite_wms_common_la_SOURCES = \
	$(configuration_cpp_files) \
	$(configuration_generated_cpp_files) \
	$(utilities_cpp_files) \
	$(logger_cpp_files)
libglite_wms_common_la_LDFLAGS = $(GLITE_LDFLAGS)
libglite_wms_common_la_LIBADD = \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
	$(BOOST_DATE_LIBS) \
	$(BOOST_FS_LIBS) \
	$(BOOST_REGEX_LIBS) \
	$(BOOST_THREAD_LIBS) \
	-llog4cpp

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(GLITE_CPPFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CPPFLAGS) \
	$(GLOBUS_THR_CFLAGS)

utilitiesincludedir = $(includedir)/glite/wms/common/utilities
utilitiesinclude_HEADERS = \
	utilities/boost_fs_add.h \
	utilities/streamdescriptor.h \
	utilities/globus_ftp_macros.h \
	utilities/globus_ftp_utils.h \
	utilities/LineParserExceptions.h \
	utilities/quota.h \
	utilities/triple.h \
	utilities/scope_guard.h \
	utilities/jobdir.h \
	utilities/input_reader.h \
	utilities/jobdir_reader.h \
	utilities/filelist_reader.h

EXTRA_DIST = \
	utilities/triple.h \
	configuration/JCConfiguration.h.G \
	configuration/LMConfiguration.h.G \
	configuration/ICEConfiguration.h.G \
	configuration/NSConfiguration.h.G \
	configuration/WMConfiguration.h.G \
        configuration/WMPConfiguration.h.G \
	configuration/CommonConfiguration.h.G \
	configuration/cwrapper.h.G \
	configuration/cwrapper.cpp.G \
	configuration/Configuration.def

configuration/Configuration.cpp: configuration/JCConfiguration.h $(configuration_generated_h_files)

configurationincludedir = $(includedir)/glite/wms/common/configuration
configurationinclude_HEADERS = \
	configuration/cwrapper.h \
	$(configuration_generated_h_files)

CLEANFILES = \
	$(configuration_generated_h_files) \
	$(configuration_generated_cpp_files)

generator_definition_file = $(srcdir)/configuration/Configuration.def

%.h: %.h.G $(generator_definition_file)
	$(top_srcdir)/src/scripts/generator.pl $(generator_definition_file) -H $<
	cp $@ $(top_srcdir)/interface/glite/wms/common/configuration

%.cpp: %.cpp.G $(generator_definition_file)
	$(top_srcdir)/src/scripts/generator.pl $(generator_definition_file) -c $<

MAINTAINERCLEANFILES = Makefile.in *~
