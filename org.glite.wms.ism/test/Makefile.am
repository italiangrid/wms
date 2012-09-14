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

check_PROGRAMS = \
  ldap_search \
  ldap_search_async \
  ii_purchaserTest \
  ii_g2_purchaserTest

ldap_search_SOURCES = \
	ldap_searchTest.cpp

ldap_search_LDADD = \
        -lldap \
        $(BOOST_LIBS)

ldap_search_async_SOURCES = \
	ldap_searchTestasync.cpp

ldap_search_async_LDADD = \
        -lldap \
        $(BOOST_LIBS)

ii_g2_purchaserTest_SOURCES = \
        ism-ii-g2-purchaserTest.cpp

ii_g2_purchaserTest_LDADD = \
        $(top_srcdir)/build/src/libglite_wms_ism.la \
        $(top_srcdir)/build/src/purchaser/libglite_wms_ism_ii_g2_purchaser.la \
        -lldap \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_JDL_LIBS) \
        $(GLITE_WMS_COMMON_LOGGER_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
        $(CLASSAD_LIBS) \
        $(BOOST_LIBS)

ii_g2_purchaserTest_CPPFLAGS = \
	-I$(top_srcdir)/interface \
	$(GLITE_WMS_COMMON_LOGGER_CFLAGS) \
	$(CLASSAD_CFLAGS) \
        $(BOOST_CFLAGS)

ii_purchaserTest_SOURCES = \
        ism-ii-purchaserTest.cpp

ii_purchaserTest_LDADD = \
        $(top_srcdir)/src/libglite_wms_ism.la \
        $(top_srcdir)/src/purchaser/libglite_wms_ism_ii_purchaser.la \
        -lldap \
	$(GLITE_WMSUTILS_CLASSADS_LIBS) \
        $(GLITE_JDL_LIBS) \
        $(GLITE_WMS_COMMON_LOGGER_LIBS) \
        $(GLITE_WMS_COMMON_CONF_LIBS) \
        $(GLITE_WMS_COMMON_UT_UTIL_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
        $(CLASSAD_LIBS) \
        $(BOOST_LIBS)


AM_CPPFLAGS = -I$(top_srcdir)/interface \	 
        -I$(top_srcdir)/src \
        -I$(top_srcidr)/test \
	$(CLASSAD_CFLAGS) \
	$(BOOST_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~
