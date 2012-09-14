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
BOOST=$(repository)/boost/1.32.0/rhel30_gcc32
CLASSAD=$(repository)/classads/0.9.7-opt/rhel30_gcc32
GLOBUS=$(repository)/globus/2.4.3-VDT-1.2.0/rhel30_gcc32
LIBTAR=$(repository)/libtar/1.2.11/rhel30_gcc32


GEN_LD_FLAGS=-L$(GLITE_REP)/lib 

WMPCLIENT_LD_FLAGS= 

WMPCLIENT_C_FLAGS= -I../  \
        -I$(CLASSAD)/include \
        -I$(GLOBUS)/include/gcc32 \
	-I$(GLOBUS)/include/gcc32/openssl \
       -I$(LIBTAR)/include \
	-I/usr/local/lib \
	-I$(stage)/include \
	-I$(BOOST)/include


all: libservices
libservices:
	@echo "compiling services....."
	$(CPP)  -Wall  -DHAVE_CONFIG_H  -DWANT_NAMESPACES -DWITH_GRID_FTP -pthread \
	$(WMPCLIENT_LD_FLAGS) $(WMPCLIENT_C_FLAGS) -shared -o libglite_wms_client_services.so  \
	lbapi.cpp job.cpp jobattach.cpp  jobcancel.cpp  joblistmatch.cpp  jobloginfo.cpp  joboutput.cpp  \
	jobstatus.cpp  jobsubmit.cpp delegateproxy.cpp listener.cpp \
	jobperusal.cpp jobinfo.cpp
	@echo "DONE"
services:
	@echo "compiling utils....."
	$(CPP)  -Wall  -DHAVE_CONFIG_H  -DWANT_NAMESPACES -DWITH_GRID_FTP -pthread \
	$(WMPCLIENT_LD_FLAGS) $(WMPCLIENT_C_FLAGS) -o glite-wmp-services \
	lbapi.cpp job.cpp jobattach.cpp  jobcancel.cpp  joblistmatch.cpp  jobloginfo.cpp  joboutput.cpp  \
	jobstatus.cpp  jobsubmit.cpp delegateproxy.cpp listener.cpp \
	jobperusal.cpp jobinfo.cpp
	@echo "DONE"
