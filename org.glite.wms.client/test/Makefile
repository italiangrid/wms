GLITE_FLAVOUR=head
REPOSITORY=/home/maraska/Repository/$(GLITE_FLAVOUR)
REPOSITORY=../../

CC=gcc
CPP=g++ -pthread
LIBS=
COFLAGS=-O2
//CWFLAGS=-Wall
CWFLAGS=
CIFLAGS=
CMFLAGS=


GLITE_REP=$(REPOSITORY)/stage/
repository=$(REPOSITORY)/repository/
GLITE_WMS_CLIENT_SRC=../src
GLOBUS_LOCATION=$(repository)/globus/2.4.3-VDT-1.2.2/rhel30_gcc32/
CLASSAD=$(repository)/classads/0.9.6/rhel30_gcc32
BOOST=$(repository)/boost/1.32.0/rhel30_gcc32/
BOOST_FLAVOUR=gcc-mt
ARES=$(repository)/ares/1.1.1/rhel30_gcc32/
LIBTAR=/usr/local/lib -ltar
ZLIB=/usr/local/lib -lz
LIBCURL=/usr/lib -lcurl
GRIDSITE_GLOBUS_LIBS=/usr/lib -lxml2 -lgridsite_globus

CPPUNIT=$(repository)/cppunit/1.10.2/rhel30_gcc32
#EXPAT=
GLOBUS_FLAVOR_THREADS=gcc32dbgpthr

CFLAGS= $(CWFLAGS) $(COFLAGS) $(CIFLAGS) $(CMFLAGS) -DDEBUG \
	-DWANT_NAMESPACES \
	-I$(CLASSAD)/include \
	-I$(BOOST)/include \
	-I$(GLITE_REP)/include \
	-I$(CPPUNIT)/include \
	-I$(GLITE_WMS_CLIENT_SRC) \
	-I. glite-wms-client-common.cpp

CDFLAGS= -L$(GLOBUS_LOCATION)/lib \
	-L$(ZLIB) \
	-L$(GLITE_REP)/lib \
	-L$(CLASSAD)/lib \
	-L$(BOOST)/lib/ \
	-L$(ARES)/lib \
	-L$(GLITE_WMS_CLIENT_SRC)/services \
	-L$(GLITE_WMS_CLIENT_SRC)/utilities \
	-L$(CPPUNIT)/lib \
	-L$(GRIDSITE_GLOBUS_LIBS) \
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
	-lglobus_common_$(GLOBUS_FLAVOR_THREADS) \
	-lglite_wms_util \
	-lglite_wmsutils_jobid \
	-lglite_wmsutils_cjobid \
	-lglite_jdl_api_cpp \
	-lglite_wmsutils_classads \
	-lglite_wmsutils_exception \
	-lclassad \
	-lboost_thread-$(BOOST_FLAVOUR)-d \
	-lboost_filesystem-$(BOOST_FLAVOUR) \
	-lboost_signals-$(BOOST_FLAVOUR)  \
	-lboost_regex-$(BOOST_FLAVOUR) \
	-lpthread \
	-lglite_wms_client_services \
	-lglite_wms_client_utils \
	-lvomsapi_gcc32dbg \
	-lgridsite_globus \
	-lglite_wms_wmproxy_api_cpp \
	-lcppunit

LDFLAGS=$(CDFLAGS) \
	-L$(LIBTAR) \
	-L$(LIBCURL) \
	-lglite_lb_common_gcc32 \
	-lglite_lb_client_gcc32 \
	-lglite_lb_clientpp_gcc32

EXTENDED=-lglite_wmsui_client \
	-lglite_dgas_jobAuthClient \
	-lglite_dgas_xmlutil \
	-lglite_wmsutils_tls_socket_pp \
	-lglite_wmsutils_tls_gsisocket_pp \
	-lglite_wms_globus_ftp_util \
	-lvomsapi_gcc32dbg \
	-lgridsite_globus \
	-lglite_lb_common_gcc32 \
	-lglite_lb_client_gcc32 \
	-lglite_lb_clientpp_gcc32 \
	-lglite_wms_checkpointing \
	-lglite_wms_logger \
	-lglite_wmsui_partitioner

all: listen output delegation

listen:
	@echo "compiling listener....."
	@$(CPP)  -Wall  -DHAVE_CONFIG_H   $(CFLAGS) -o glite-wms-client-listenertest glite-wms-client-listener.cpp $(LDFLAGS)
	@echo "DONE glite-wms-client-listenertest"
output:
	@echo "compiling output....."
	@$(CPP)  -Wall  -DHAVE_CONFIG_H   $(CFLAGS) -o glite-wms-client-outputtest glite-wms-client-output.cpp $(LDFLAGS)
	@echo "DONE glite-wms-client-outputtest"

delegation:
	@echo "compiling delegation....."
	$(CPP)  -Wall  -DHAVE_CONFIG_H   $(CFLAGS) -o glite-wms-client-delegationtest glite-wms-client-delegation.cpp $(LDFLAGS)
	@echo "DONE glite-wms-client-delegationtest"




