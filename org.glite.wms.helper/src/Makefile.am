#  Copyright (c) Members of the EGEE Collaboration. 2004. 
#  #  See http://www.eu-egee.org/partners/ for details on the copyright
#  holders.  
#  #  
#  Licensed under the Apache License, Version 2.0 (the "License"); 
#  #  you may not use this file except in compliance with the License. 
#  You may obtain a copy of the License at 
#  #  
#      http://www.apache.org/licenses/LICENSE-2.0 
#      #  
#  Unless required by applicable law or agreed to in writing, software 
#  #  distributed under the License is distributed on an "AS IS" BASIS, 
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
#  #  See the License for the specific language governing permissions and 
#  limitations under the License.

SUBDIRS = jobadapter broker

lib_LTLIBRARIES = libglite_wms_helper.la

helperdir = $(includedir)/glite/wms/helper
helper_HEADERS = \
	CopyHelper.h \
	Helper.h \
	Request.h \
	RequestStateMachine.h

libglite_wms_helper_la_SOURCES = \
        Helper.h \
        Helper.cpp \
        exceptions.cpp \
        HelperFactory.cpp \
        HelperImpl.cpp \
        Request.h \
        Request.cpp \
        RequestStateMachine.h \
        RequestStateMachine.cpp

libglite_wms_helper_la_LIBADD = \
	$(BOOST_LIBS) \
	$(CLASSAD_LIBS) \
	$(GLITE_JDL_LIBS) \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) 

AM_CPPFLAGS = -I$(top_srcdir)/interface \
	-I$(top_srcdir)/src \
	$(GLITE_JDL_CFLAGS) \
	$(GLITE_WMSUTILS_CLASSADS_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(CLASSAD_CFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wms-helper.pc \
		wms-helper-jobadapter.pc \
		wms-helper-broker-ism.pc

MAINTAINERCLEANFILES = Makefile.in
