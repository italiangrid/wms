## LICENSE:
## Copyright (c) Members of the EGEE Collaboration. 2010. 
## See http://www.eu-egee.org/partners/ for details on the copyright
## holders.  

## Licensed under the Apache License, Version 2.0 (the "License"); 
## you may not use this file except in compliance with the License. 
## You may obtain a copy of the License at 

##   http://www.apache.org/licenses/LICENSE-2.0 

## Unless required by applicable law or agreed to in writing, software 
## distributed under the License is distributed on an "AS IS" BASIS, 
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
## implied. 
## See the License for the specific language governing permissions and 
## limitations under the License.

## END LICENSE

lib_LTLIBRARIES = libglite_wms_classad_plugin_loader.la libglite_wms_classad_plugin.la

classad_plugindir = $(includedir)/glite/wms/classad_plugin

classad_plugin_HEADERS = \
	classad_plugin_loader.h
 
libglite_wms_classad_plugin_loader_la_SOURCES = \
	classad_plugin_loader.cpp

libglite_wms_classad_plugin_la_SOURCES = \
        classad_plugin.cpp \
        doMatch.cpp \
        listAttrRegEx.cpp \
        measured_response_time.cpp \
        successFraction.cpp \
        testMemberEx.cpp \
        retrieveCloseSEsInfo.cpp

libglite_wms_classad_plugin_la_LIBADD = $(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(CLASSAD_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_ISM_LIBS) \
        $(GLITE_LB_CLIENT_LIBS) \
        $(BOOST_LIBS)

libglite_wms_classad_plugin_la_CXXFLAGS = -fPIC -shared

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	$(GLITE_WMSUTILS_CLASSADS_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	$(GLITE_WMS_COMMON_CONF_CFLAGS) \
	$(GLITE_WMS_ISM_CFLAGS) \
	$(GLITE_LB_CLIENT_CFLAGS) \
	$(BOOST_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in
