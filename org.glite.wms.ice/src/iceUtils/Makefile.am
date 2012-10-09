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

lib_LTLIBRARIES = libglite_wms_iceUtils.la

libglite_wms_iceUtils_la_SOURCES = \
				IceUtils.cpp \
				CreamJob.cpp \
				ESActivity.cpp \
				DNProxyManager.cpp \
				DelegationManager.cpp \
				CEBlackList.cpp \
				CreamProxyMethod.cpp \
				IceConfManager.cpp \
				IceLBContext.cpp \
				iceLBEventFactory.cpp \
				IceLBEvent.cpp \
				iceLBLogger.cpp \
				Request_jobdir.cpp \
				Request_source_jobdir.cpp \
				RequestParser.cpp \
				Url.cpp

AM_CPPFLAGS =  \
		-I$(top_srcdir)/src \
                $(VOMS_CFLAGS) \
		$(CLASSAD_CFLAGS) \
		$(GSOAP_CFLAGS) \
		$(LOG4CPP_CFLAGS) \
		$(GLITE_WMS_COMMON_CONF_CFLAGS) \
		$(BOOST_CFLAGS) \
		-UHAVE_CONFIG_H \
		-DGLITE_WMS_HAVE_LBPROXY \
		-pipe

MAINTAINERCLEANFILES = Makefile.in
