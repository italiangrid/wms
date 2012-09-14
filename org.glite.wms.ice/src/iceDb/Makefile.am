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

## END LICENSE */

SUBDIRS = sqlite

lib_LTLIBRARIES = libglite_wms_iceDb.la

libglite_wms_iceDb_la_SOURCES = Transaction.cpp \
				GetCEUrl.cpp \
				AbsDbOperation.cpp \
				CheckGridJobID.cpp \
				CheckDelegationByID.cpp \
				GetAllProxyByDN.cpp \
				GetDelegation.cpp \
				GetDelegationByID.cpp \
				GetAllDelegation.cpp \
				GetProxyInfoByDN.cpp \
				GetProxyInfoByDN_MYProxy.cpp \
				GetLease.cpp \
				GetLeaseByID.cpp \
				GetTerminatedJobs.cpp \
				CreateJob.cpp \
				GetAllProxyInfo.cpp \
				CreateProxyField.cpp \
				CreateDelegation.cpp \
				CreateLease.cpp \
				DNHasJobs.cpp \
				UpdateProxyFieldsByDN.cpp \
				UpdateDelegationTimesByID.cpp \
				RemoveJobByGid.cpp \
				RemoveJobByCid.cpp \
				RemoveProxyByDN.cpp \
				RemoveDelegationByID.cpp \
				RemoveDelegationByDNMyProxy.cpp \
				RemoveDelegation.cpp \
				RemoveLease.cpp \
				GetAllJobs.cpp \
				GetJobByGid.cpp \
				GetJobByCid.cpp \
				GetJobsToPoll.cpp \
				GetJobsByDbID.cpp \
				GetEventID.cpp \
				SetEventID.cpp \
				SetEventIDForCE.cpp \
				GetDbID.cpp \
				SetDbID.cpp \
				GetAllDN.cpp \
				RemoveJobsByDbID.cpp \
				InsertStat.cpp \
				GetStats.cpp \
				GetJobsByDN.cpp \
				GetJobsByDNMyProxy.cpp \
				UpdateProxyCounterByDN.cpp \
				RemoveJobByUserDN.cpp \
				UpdateJob.cpp

AM_CPPFLAGS =  \
		-I$(top_srcdir)/src \
		-I$(top_srcdir)/src/iceDb/sqlite \
                $(VOMS_CFLAGS) \
		$(CLASSAD_CFLAGS) \
		$(GSOAP_CFLAGS) \
		$(LOG4CPP_CFLAGS) \
		$(BOOST_CFLAGS) \
		-UHAVE_CONFIG_H \
		-DGLITE_WMS_HAVE_LBPROXY \
		-pipe

MAINTAINERCLEANFILES = Makefile.in
