# Copyright (c) Members of the EGEE Collaboration. 2004.
# See http://www.eu-egee.org/partners/ for details on the copyright
# holders.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

lib_LTLIBRARIES = libglite_wms_jss_common.la

noinst_HEADERS = \
	constants.h \
	fstreamlock.h \
	files.h \
	file_container.h \
	file_locker.h \
	filelist.h \
	filelist_lock.h \
	filelist_reader.h \
	filelist_iterator.h \
	filelist_extractor.h \
	pointer_id.h \
	id_compare.h \
	id_container.h \
	ram_container.h \
	EventLogger.h \
	JobFilePurger.h \
	ProxyUnregistrar.h \
	LockFile.h \
	SignalChecker.h \
	handler.h

libglite_wms_jss_common_la_SOURCES = \
	fstreamlock.cpp \
	id_container.cpp \
	ram_container.cpp \
	file_container.cpp \
	pointer_id.cpp \
	filelist.cpp \
	filelist_iterator.cpp \
	filelist_lock.cpp \
	file_locker.cpp \
	EventLogger.cpp \
	JobFilePurger.cpp \
	ProxyUnregistrar.cpp \
	constants.cpp \
	files.cpp \
	LockFile.cpp \
	SignalChecker.cpp \
	handler.c

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GLITE_WMS_COMMON_UT_UTIL_CFLAGS) \
	-I/usr/include \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~
