CC=gcc
CPP=g++
LIBS=
COFLAGS=-O2
//CWFLAGS=-Wall
CWFLAGS=
CIFLAGS=
CMFLAGS=


REPOSITORY=../../../
repository=$(REPOSITORY)/repository
stage=$(REPOSITORY)/stage


BOOST=$(repository)/boost/1.32.0-slc3/rhel30_gcc32
BOOST_SUFFIX=-gcc-mt-d
BOOST_SUFFIX=
CLASSAD=$(repository)/classads/0.9.7-opt/rhel30_gcc32
GLOBUS=$(repository)/globus/2.4.3-VDT-1.2.2/rhel30_gcc32/
GLOBUS_FLAVOR=gcc32dbg


WMPCLIENT_LD_FLAGS= \
	-L$(BOOST)/lib \
	-L$(CLASSAD)/lib \
	-L$(stage)/lib \
	-lglite_wmsutils_exception \
	-lglite_wms_util \
	-lglite_wms_conf \
	-lglite_wmsutils_jobid  \
	-lglite_wmsutils_cjobid \
	-lglite_jdl_api_cpp \
	-lclassad



WMPCLIENT_LD_EXE_FLAGS= \
	$(WMPCLIENT_LD_FLAGS) \
	-L$(GLOBUS)/lib \
	-lglite_wmsutils_classads \
	-lboost_filesystem$(BOOST_SUFFIX) \
	-lboost_thread$(BOOST_SUFFIX) \
	-lboost_signals$(BOOST_SUFFIX) \
	-lboost_regex$(BOOST_SUFFIX) \
	-lz -lpthread -lm \
	-lvomsapi_gcc32dbg \
	-lgridsite_globus \
	-lglite-sd-c \
	-lglite_wms_wmproxy_api_cpp \
	-lxml2

WMPCLIENT_C_FLAGS= \
	-I$(GLOBUS)/include/gcc32/ \
	-I$(BOOST)/include \
	-I$(CLASSAD)/include \
	-I$(stage)/include


WMPCLIENT_FILES= utils.cpp \
	adutils.cpp \
	excman.cpp \
 	logman.cpp \
	options_utils.cpp

all: libutils
libutils:
	@echo "compiling utils....."
	$(CPP)  -Wall  -DHAVE_CONFIG_H  -DWANT_NAMESPACES -DWITH_OPENSSL \
	$(WMPCLIENT_LD_FLAGS) $(WMPCLIENT_C_FLAGS) -shared -o libglite_wms_client_utils.so  $(WMPCLIENT_FILES)
	@echo "DONE"
utils:
	@echo "compiling utils....."
	$(CPP)  -Wall  -DHAVE_CONFIG_H  -DWANT_NAMESPACES \
	$(WMPCLIENT_LD_EXE_FLAGS) $(WMPCLIENT_C_FLAGS) -o glite-wmp-utils glite-wmp-utils.cpp $(WMPCLIENT_FILES)
	@echo "DONE"
