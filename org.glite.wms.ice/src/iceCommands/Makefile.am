# LICENSE:
# Copyright (c) Members of the EGEE Collaboration. 2010. 
# See http://www.eu-egee.org/partners/ for details on the copyright
# holders.  

# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 

#   http://www.apache.org/licenses/LICENSE-2.0 

# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. 
# See the License for the specific language governing permissions and 
# limitations under the License.

# END LICENSE */

lib_LTLIBRARIES = libglite_wms_iceCommands.la

libglite_wms_iceCommands_la_SOURCES = \
	iceCommandSubmit.cpp \
	iceCommandCancel.cpp \
	iceCommandFactory.cpp \
	iceCommandStatusPoller.cpp \
	Request_source_purger.cpp \
	iceCommandDelegationRenewal.cpp \
	iceCommandEventQuery.cpp \
	iceCommandLBLogging.cpp \
	iceCommandReschedule.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src \
		$(VOMS_CFLAGS) \
		$(LOG4CPP_CFLAGS) \
		$(CLASSAD_CFLAGS) \
		$(GSOAP_CFLAGS) \
		$(BOOST_CFLAGS) \
		$(LOG4CPP_CFLAGS) \
		-UHAVE_CONFIG_H \
		-DGLITE_WMS_ICE_ENABLE_STATS \
		-pipe

MAINTAINERCLEANFILES = Makefile.in
