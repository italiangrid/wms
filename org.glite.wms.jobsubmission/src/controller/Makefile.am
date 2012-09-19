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

lib_LTLIBRARIES = libglite_wms_jss_controller.la \
	libglite_wms_jss_controller_wrapper.la \
	libglite_wms_jss_controller_adapter.la

noinst_HEADERS = \
	CondorG.h \
	cwrapper.h \
	JobControllerExceptions.h \
	JobControllerFactory.h \
	JobControllerFake.h \
	JobController.h \
	JobControllerImpl.h \
	JobControllerProxy.h \
	JobControllerReal.h \
	JobControllerClient.h \
	JobControllerClientImpl.h \
	JobControllerClientJD.h \
	Request.h \
	RequestExceptions.h \
	SubmitAd.h \
	SubmitAdExceptions.h

libglite_wms_jss_controller_wrapper_la_SOURCES = cwrapper.cpp

libglite_wms_jss_controller_adapter_la_SOURCES = SubmitAd.cpp \
	SubmitAdExceptions.cpp \
	SubmitAdapter.cpp

libglite_wms_jss_controller_la_SOURCES = CondorG.cpp \
	Request.cpp \
	RequestExceptions.cpp \
	JobController.cpp \
	JobControllerExceptions.cpp \
	JobControllerFactory.cpp \
	JobControllerProxy.cpp \
	JobControllerReal.cpp \
	JobControllerFake.cpp \
	JobControllerClient.cpp \
	JobControllerClientJD.cpp


libglite_wms_jss_controller_la_LIBADD = ../common/libglite_wms_jss_common.la \
					$(CONDORG_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/src \
	-I$(top_srcdir)/interface \
	-I/usr/include \
	$(GLITE_WMS_COMMON_CONF_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(CONDORG_CFLAGS) -DCONDORG_VERSION=$(CONDORG_VERSION)

MAINTAINERCLEANFILES = Makefile.in *~
