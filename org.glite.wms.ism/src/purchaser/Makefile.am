## Copyright (c) Members of the EGEE Collaboration. 2009. 
## See http://www.eu-egee.org/partners/ for details on the copyright holders.  

## Licensed under the Apache License, Version 2.0 (the "License"); 
## you may not use this file except in compliance with the License. 
## You may obtain a copy of the License at 
##     http://www.apache.org/licenses/LICENSE-2.0 
## Unless required by applicable law or agreed to in writing, software 
## distributed under the License is distributed on an "AS IS" BASIS, 
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
## See the License for the specific language governing permissions and 
## limitations under the License.

lib_LTLIBRARIES    = libglite_wms_ism_ii_purchaser.la

libglite_wms_ism_ii_purchaser_la_SOURCES = \
        common.cpp \
        ldap-utils.cpp \
        ldap-dn-utils.cpp \
        schema_utils.cpp \
        ism-ii-purchaser.cpp \
        ldap-utils-g2.cpp \
        ldap-dn-utils-g2.cpp \
        schema_utils-g2.cpp

libglite_wms_ism_ii_purchaser_la_LIBADD = $(GLITE_WMSUTILS_CLASSADS_LIBS) \
                $(GLITE_WMS_COMMON_LOGGER_LIBS) \
                $(BOOST_LIBS) \
                $(CLASSAD_LIBS) \
                $(LDAP_LIBS)

libglite_wms_ism_file_purchaser_la_SOURCES = \
        common.cpp \
        ism-file-purchaser.cpp 

libglite_wms_ism_file_purchaser_la_LIBADD = $(GLITE_WMSUTILS_CLASSADS_LIBS) \
                $(GLITE_WMS_COMMON_LOGGER_LIBS) \
                $(BOOST_LIBS) \
                $(CLASSAD_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
		-I$(top_srcdir)/src \
		$(GLITE_WMSUTILS_CLASSADS_CFLAGS) \
		$(GLITE_WMS_COMMON_LOGGER_CFLAGS) \
		$(BOOST_CFLAGS) \
		$(CLASSAD_CFLAGS) \
		$(LDAP_CPPFLAGS)

MAINTAINERCLEANFILES = Makefile.in *.*~
