cc=Gcc
CPP=g++
LIBS=
COFLAGS=-O2
CWFLAGS=
CIFLAGS=-I../..
CMFLAGS=

COFLAGS=-O2
CWFLAGS=
#CWFLAGS=-Wall

CMFLAGS=


top_srcdir= ..
src_dir = $(top_srcdir)/src
bin_dir = $(top_srcdir)/bin
lib_dir = $(top_srcdir)/lib

REPOSITORY = ../../..
GLOBUS_FLAVOR=gcc32dbg
GLOBUS_FLAVOR_THREADS=gcc32dbgpthr

GLOBUS_LOCATION=$(REPOSITORY)/repository/globus/2.4.3-VDT-1.2.2/rhel30_gcc32
BOOST_LOCATION=$(REPOSITORY)/repository/boost/1.32.0/rhel30_gcc32
EXPAT_LOCATION=$(REPOSITORY)/repository/expat/1.95.7/rhel30_gcc32/

EXPAT=$(REPOSITORY)/repository/expat/1.95.7/rhel30_gcc32
CLASSAD=$(REPOSITORY)/repository/classads/0.9.6/rhel30_gcc32
BOOST=$(REPOSITORY)/repository/boost/1.32.0/rhel30_gcc32
ARES=$(REPOSITORY)/repository/c-ares/1.3.0/rhel30_gcc32


BOOST_LIBS=-L$(BOOST_LOCATION)/lib \
-lboost_filesystem-gcc-mt -lboost_regex-gcc-mt -lboost_thread-gcc-mt -lpthread 
CLASSAD_LIBS=-L$(CLASSAD)/lib  -lclassad -lclassad_dl
GSI_PLUGIN=/home/msotty/webservices/gsoap/gsoap-gsi-plugin-2.0

CIFLAGS=-I../ \
-I$(BOOST)/include \
-I$(REPOSITORY)/stage/include \
-I$(CLASSAD)/include 

CFLAGS= -DDEBUG -DWANT_NAMESPACES -DWITH_OPENSSL\
$(CWFLAGS) $(COFLAGS) $(CIFLAGS) $(CMFLAGS) -pthread

JDL_LIB=-lglite_jdl_api_cpp

JOBID_LIB=-lglite_wmsutils_jobid \
-lglite_wmsutils_cjobid
EXC_LIB=-lglite_wmsutils_exception

#GRIDSITE_LIBS=-lgridsite -lxml2
GRIDSITE_GLOBUS_STAGE_LIBS=-lgridsite_globus
GRIDSITE_GLOBUS_LIBS = -L/usr/lib -lxml2
LIBTAR=-L/usr/local/lib -ltar
ZLIB=-L/usr/local/lib -lz

LB_LIBRARIES=\
-L$(EXPAT)/lib \
-L$(ARES)/lib \
-L$(GLOBUS_LOCATION)/lib \
-lexpat \
-lcares \
-lglobus_gsi_proxy_core_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_gsi_credential_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_gsi_callback_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_oldgaa_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_gsi_sysconfig_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_gsi_cert_utils_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_openssl_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_proxy_ssl_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_openssl_error_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_ftp_client_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_ftp_control_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_io_$(GLOBUS_FLAVOR_THREADS) \
-lssl_$(GLOBUS_FLAVOR_THREADS) \
-lcrypto_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_gss_assist_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_gssapi_gsi_$(GLOBUS_FLAVOR_THREADS) \
-lglobus_common_$(GLOBUS_FLAVOR_THREADS)

STAGELIBS=-L$(REPOSITORY)/stage/lib \
-lglite_wmsutils_classads \
-lglite_wms_util \
-lglite_wms_wmproxy_api_cpp \
-lglite_wms_util \
-lglite_wms_client_utils \
-lglite_wms_client_services \
-lglite_lb_common_gcc32 \
-lglite_lb_client_gcc32 \
-lglite_lb_clientpp_gcc32 \
-lglite_wms_globus_ftp_util \
-lglite_wms_logger \
-lglite_security_gss_gcc32 \
-lvomsapi_gcc32 \
$(LB_LIBRARIES) \
$(GRIDSITE_GLOBUS_STAGE_LIBS) \
$(GRIDSITE_GLOBUS_LIBS) \
$(LIBTAR) \
$(ZLIB)

COMMON_LD_FLAGS=\
-L../services \
-L../utilities \
$(STAGELIBS) \
$(GRIDSITE_GLOBUS_LIBS) \
$(BOOST_LIBS)


job_submit_SOURCE=wmp-job-submit.cpp
job_submit_OBJ=glite-wms-job-submit
job_submit_CFLAGS=$(CFLAGS)
job_submit_DEPS=$(COMMON_LD_FLAGS)\
$(JDL_LIB)

job_status_SOURCE=wmp-job-status.cpp
job_status_OBJ=glite-wms-job-status
job_status_CFLAGS=$(CFLAGS)
job_status_DEPS=$(COMMON_LD_FLAGS)

job_loginfo_SOURCE=wmp-job-logging-info.cpp
job_loginfo_OBJ=glite-wms-job-logging-info
job_loginfo_CFLAGS=$(CFLAGS)
job_loginfo_DEPS=$(COMMON_LD_FLAGS)

job_cancel_SOURCE=wmp-job-cancel.cpp
job_cancel_OBJ=glite-wms-job-cancel
job_cancel_CFLAGS=$(CFLAGS)
job_cancel_DEPS=$(COMMON_LD_FLAGS) 

job_match_SOURCE=wmp-job-list-match.cpp
job_match_OBJ=glite-wms-job-list-match
job_match_CFLAGS=$(CFLAGS)
job_match_DEPS=$(COMMON_LD_FLAGS)\
$(JDL_LIB)

job_output_SOURCE=wmp-job-output.cpp
job_output_OBJ=glite-wms-job-output
job_output_CFLAGS=$(CFLAGS)
job_output_DEPS=$(COMMON_LD_FLAGS) 

job_attach_SOURCE=wmp-job-attach.cpp
job_attach_OBJ=glite-wms-job-attach
job_attach_CFLAGS=$(CFLAGS)
job_attach_DEPS=$(COMMON_LD_FLAGS)

delegate_proxy_SOURCE=wmp-job-delegate-proxy.cpp
delegate_proxy_OBJ=glite-wms-job-delegate-proxy
delegate_proxy_CFLAGS=$(CFLAGS)
delegate_proxy_DEPS=$(COMMON_LD_FLAGS) 

proxyinfo_SOURCE=wmp-job-info.cpp
proxyinfo_OBJ=glite-wms-job-info
proxyinfo_CFLAGS=$(CFLAGS)
proxyinfo_DEPS=$(COMMON_LD_FLAGS)

job_perusal_SOURCE=wmp-job-perusal.cpp
job_perusal_OBJ=glite-wms-job-perusal
job_perusal_CFLAGS=$(CFLAGS)
job_perusal_DEPS=$(COMMON_LD_FLAGS)

all: submit cancel match output  delegation proxy_info perusal #status loginfo attach

submit:	$(job_submit_SOURCE)
		@echo "building "$(job_submit_OBJ)" ..."

		@if ! test -d $(bin_dir) ; then \
			mkdir $(bin_dir) ; \
		fi

		$(CPP) -g -Wall $(job_submit_CFLAGS) $(job_submit_DEPS)\
			-o  $(job_submit_OBJ)  \
			$(job_submit_SOURCE)
		@mv $(job_submit_OBJ) $(bin_dir)

status:	$(job_status_SOURCE)
		@echo "building "$(job_status_OBJ)" ..."

		@if ! test -d $(bin_dir) ; then \
			mkdir $(bin_dir) ; \
		fi
		$(CPP) -g -Wall  $(job_status_CFLAGS) $(job_status_DEPS) \
			-o  $(job_status_OBJ)  \
			$(job_status_SOURCE)
		@mv $(job_status_OBJ) $(bin_dir)

loginfo:	$(job_loginfo_SOURCE)
		@echo "building "$(job_loginfo_OBJ)" ..."

		@if ! test -d $(bin_dir) ; then \
			mkdir $(bin_dir) ; \
		fi
		$(CPP) -g -Wall  $(job_loginfo_CFLAGS) $(job_loginfo_DEPS) \
			-o  $(job_loginfo_OBJ)  \
			$(job_loginfo_SOURCE)
		@mv $(job_loginfo_OBJ) $(bin_dir)

cancel:	$(job_cancel_SOURCE)
		@echo "building "$(job_cancel_OBJ)" ..."

		@if ! test -d $(bin_dir) ; then \
			mkdir $(bin_dir) ; \
		fi
		$(CPP) -g -Wall  $(job_cancel_CFLAGS) $(job_cancel_DEPS) \
			-o  $(job_cancel_OBJ)  \
			$(job_cancel_SOURCE)
		@mv $(job_cancel_OBJ) $(bin_dir)

match:	$(job_match_SOURCE)
		@echo "building "$(job_match_OBJ)" ..."

		@if ! test -d $(bin_dir) ; then \
			mkdir $(bin_dir) ; \
		fi

		$(CPP) -g -Wall  $(job_match_CFLAGS) $(job_match_DEPS) \
			-o  $(job_match_OBJ)  \
			$(job_match_SOURCE)
		@mv $(job_match_OBJ) $(bin_dir)

output:	$(job_output_SOURCE)
		@echo "building "$(job_output_OBJ)" ..."

		@if ! test -d $(bin_dir) ; then \
			mkdir $(bin_dir) ; \
		fi
		$(CPP) -g -Wall  $(job_output_CFLAGS) $(job_output_DEPS) \
			-o  $(job_output_OBJ)  \
			$(job_output_SOURCE)
		@mv $(job_output_OBJ) $(bin_dir)

attach:	$(job_attach_SOURCE)
		@echo "building "$(job_attach_OBJ)" ..."

		@if ! test -d $(bin_dir) ; then \
			mkdir $(bin_dir) ; \
		fi
		$(CPP) -g -Wall  $(job_attach_CFLAGS) $(job_attach_DEPS) \
			-o  $(job_attach_OBJ)  \
			$(job_attach_SOURCE)
		@mv $(job_attach_OBJ) $(bin_dir)

delegation:	 $(delegate_proxy_SOURCE)
		@echo "building "$(delegate_proxy_OBJ)" ..."

		@if ! test -d $(bin_dir) ; then \
			mkdir $(bin_dir) ; \
		fi
		$(CPP) -g -Wall  $(delegate_proxy_CFLAGS) $(delegate_proxy_DEPS) \
			-o  $(delegate_proxy_OBJ)  \
			$(delegate_proxy_SOURCE)
		@mv $(delegate_proxy_OBJ) $(bin_dir)

proxy_info:	 $(proxyinfo_SOURCE)
		@echo "building "$(proxyinfo_OBJ)" ..."

		@if ! test -d $(bin_dir) ; then \
			mkdir $(bin_dir) ; \
		fi
		$(CPP) -g -Wall  $(proxyinfo_CFLAGS) $(proxyinfo_DEPS) \
			-o  $(proxyinfo_OBJ)  \
			$(proxyinfo_SOURCE)
		@mv $(proxyinfo_OBJ) $(bin_dir)

perusal:	$(job_perusal_SOURCE)
		@echo "building "$(job_perusal_OBJ)" ..."

		@if ! test -d $(bin_dir) ; then \
			mkdir $(bin_dir) ; \
		fi
		$(CPP) -g -Wall  $(job_perusal_CFLAGS) $(job_perusal_DEPS) \
			-o  $(job_perusal_OBJ)  \
			$(job_perusal_SOURCE)
		@mv $(job_perusal_OBJ) $(bin_dir)

clean:
		rm -f *.o *.so

distclean:
		rm -f *.o *.so
