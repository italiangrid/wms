#
#       Copyright (c) 2004 on behalf of the EU EGEE Project:
#       The European Organization for Nuclear Research (CERN),
#       Istituto Nazionale di Fisica Nucleare (INFN), Italy
#       Datamat Spa, Italy
#       Centre National de la Recherche Scientifique (CNRS), France
#       CS Systeme d'Information (CSSI), France
#       Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
#       Universiteit van Amsterdam (UvA), Netherlands
#       University of Helsinki (UH.HIP), Finland
#       University of Bergen (UiB), Norway
#       Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom
#
#       Authors: Giuseppe Avellino <giuseppe.avellino@datamat.it>
#
# WARNING: work in progress


WMPROXY_API_CPP = $(top_builddir)/src/libglite_wms_wmproxy_api_cpp.la

WMS_COMMON_UT_UTIL_LIBS = -L$(libdir) -lglite_wms_util
WMSUTILS_JOBID_LIBS = -lglite_wmsutils_jobid
WMSUTILS_CJOBID_LIBS = -lglite_wmsutils_cjobid 
WMSUTILS_EXCEPTION = -lglite_wmsutils_exception 
WMS_JDL_LIBS = -lglite_wms_jdl
#WMS_CHKPT_LIBS = -lglite_wms_checkpointing
CURL = -L/usr/lib -lcurl


wmpex_bindir = $(prefix)/examples
wmpex_bin_PROGRAMS = \
	glite-wmp-job-delegate-proxy \
	glite-wmp-job-register \
	glite-wmp-job-start \
	glite-wmp-job-submit \
	glite-wmp-job-cancel \
	glite-wmp-job-output

glite_wmp_job_delegate_proxy_SOURCES = \
	glite-wmp-job-delegate-proxy.cpp \
	glite-wmp-job-utilities.cpp
glite_wmp_job_delegate_proxy_LDFLAGS = -Xlinker --export-dynamic
glite_wmp_job_delegate_proxy_LDADD = \
	$(WMPROXY_API_CPP) \
	$(WMS_COMMON_UT_UTIL_LIBS) \
	$(WMSUTILS_JOBID_LIBS) \
	$(WMSUTILS_CJOBID_LIBS) \
	$(WMSUTILS_EXCEPTION) \
	$(WMS_JDL_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS) \
	$(GRIDSITE_LIBS) \
	$(GRIDSITE_OPENSSL_LIBS)

#glite_wmp_job_register_bin_PROGRAMS = glite-wmp-job-register
glite_wmp_job_register_SOURCES = \
	glite-wmp-job-register.cpp \
	glite-wmp-job-utilities.cpp
glite_wmp_job_register_LDFLAGS = -Xlinker --export-dynamic
glite_wmp_job_register_LDADD = \
	$(WMPROXY_API_CPP) \
	$(WMS_COMMON_UT_UTIL_LIBS) \
	$(WMSUTILS_JOBID_LIBS) \
	$(WMSUTILS_CJOBID_LIBS) \
	$(WMSUTILS_EXCEPTION) \
	$(WMS_JDL_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS) \
	$(GRIDSITE_LIBS) \
	$(GRIDSITE_OPENSSL_LIBS)

#glite_wmp_job_start_bin_PROGRAMS = glite-wmp-job-start
glite_wmp_job_start_SOURCES = \
	glite-wmp-job-start.cpp \
	glite-wmp-job-utilities.cpp
glite_wmp_job_start_LDFLAGS = -Xlinker --export-dynamic
glite_wmp_job_start_LDADD = \
	$(WMPROXY_API_CPP) \
	$(WMS_COMMON_UT_UTIL_LIBS) \
	$(WMSUTILS_JOBID_LIBS) \
	$(WMSUTILS_CJOBID_LIBS) \
	$(WMSUTILS_EXCEPTION) \
	$(WMS_JDL_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS) \
	$(GRIDSITE_LIBS) \
	$(GRIDSITE_OPENSSL_LIBS)

#glite_wmp_job_submit_bin_PROGRAMS = glite-wmp-job-submit
glite_wmp_job_submit_SOURCES = \
	glite-wmp-job-submit.cpp \
	glite-wmp-job-utilities.cpp
glite_wmp_job_submit_LDFLAGS = -Xlinker --export-dynamic
glite_wmp_job_submit_LDADD = \
	$(WMPROXY_API_CPP) \
	$(WMS_COMMON_UT_UTIL_LIBS) \
	$(WMSUTILS_JOBID_LIBS) \
	$(WMSUTILS_CJOBID_LIBS) \
	$(WMSUTILS_EXCEPTION) \
	$(WMS_JDL_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS) \
	$(CURL) \
	$(GRIDSITE_LIBS) \
	$(GRIDSITE_OPENSSL_LIBS)


#glite_wmp_job_cancel_bin_PROGRAMS = glite-wmp-job-cancel
glite_wmp_job_cancel_SOURCES = \
	glite-wmp-job-cancel.cpp \
	glite-wmp-job-utilities.cpp
glite_wmp_job_cancel_LDFLAGS = -Xlinker --export-dynamic
glite_wmp_job_cancel_LDADD = \
	$(WMPROXY_API_CPP) \
	$(WMS_COMMON_UT_UTIL_LIBS) \
	$(WMSUTILS_JOBID_LIBS) \
	$(WMSUTILS_CJOBID_LIBS) \
	$(WMSUTILS_EXCEPTION) \
	$(WMS_JDL_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS) \
	$(GRIDSITE_LIBS) \
	$(GRIDSITE_LIBS) \
	$(GRIDSITE_OPENSSL_LIBS)

#glite_wmp_job_output_bin_PROGRAMS = glite-wmp-job-output
glite_wmp_job_output_SOURCES = \
	glite-wmp-job-output.cpp \
	glite-wmp-job-utilities.cpp
glite_wmp_job_output_LDFLAGS = -Xlinker --export-dynamic
glite_wmp_job_output_LDADD = \
	$(WMPROXY_API_CPP) \
	$(WMS_COMMON_UT_UTIL_LIBS) \
	$(WMSUTILS_JOBID_LIBS) \
	$(WMSUTILS_CJOBID_LIBS) \
	$(WMSUTILS_EXCEPTION) \
	$(WMS_JDL_LIBS) \
	$(CLASSAD_LIBS) \
	$(BOOST_LIBS) \
	$(CURL) \
	$(GRIDSITE_LIBS) \
	$(GRIDSITE_OPENSSL_LIBS)



AM_CPPFLAGS = -I$(top_srcdir)/interface \
	$(BOOST_CFLAGS) \
	$(GRIDSITE_CFLAGS) \
	$(GRIDSITE_OPENSSL_CFLAGS) \
	$(CLASSAD_CFLAGS) \
	-DWANT_NAMESPACES -DWITH_OPENSSL



MAINTAINERCLEANFILES = Makefile.in
