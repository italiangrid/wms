SOAPPARSER=../wsdl2h
GSOAP=../soapcpp2
SOAPH=../stdsoap2.h
SOAPC=../stdsoap2.c
SOAPCPP=../stdsoap2.cpp
SOAPCPPOBJ=stdsoap2.o
CC=gcc
CPP=g++
LIBS=
COFLAGS=-O2
CWFLAGS=
#CWFLAGS=-Wall
CIFLAGS=-I../.. -I..
CMFLAGS=
CFLAGS= $(CWFLAGS) $(COFLAGS) $(CIFLAGS) $(CMFLAGS) -DDEBUG

GLOBUS_FLAVOR=gcc32dbg
GLOBUS_FLAVOR_THREADS=gcc32dbgpthr
BOOST=../../repository/boost/1.32.0-slc3
#LDFLAGS= #-L$(GLOBUS_LOCATION)/lib \
	#-L$(GSI_PLUGIN)/src/.libs \
	#-L$(SSL_HELPERS)/.libs \
	#-ledg_wl_ssl_helpers \
	#-ledg_wl_ssl_pthr_helpers \
	#-lpthread \
	#-lgsigsoapthreads \
  	#-lglobus_gss_assist_$(GLOBUS_FLAVOR_THREADS) \
   	#-lglobus_gssapi_gsi_$(GLOBUS_FLAVOR_THREADS) \
	#-lglobus_common_$(GLOBUS_FLAVOR_THREADS)

GRIDSITE_GLOBUS_STAGE_LIBS=-lgridsite_globus
GRIDSITE_GLOBUS_LIBS = -L/usr/lib -lxml2

LDFLAGS=-L../../stage/lib \
	-lglite_wms_wmproxy_api_cpp \
	 -lssl -lcrypto \
	$(GRIDSITE_GLOBUS_STAGE_LIBS) \
	$(GRIDSITE_GLOBUS_LIBS) \
	-L$(GLOBUS_LOCATION)/lib \
  	-lglobus_gss_assist_$(GLOBUS_FLAVOR_THREADS) \
   	-lglobus_gssapi_gsi_$(GLOBUS_FLAVOR_THREADS) \
	-lglobus_common_$(GLOBUS_FLAVOR_THREADS)

all: client
cli: lib client
allinone: wsdl soap lib client


client:
	$(CPP) $(CFLAGS) -I$(BOOST)/include -DWITH_OPENSSL\
	-o test.exe test.cpp $(LIBS) \
	-I ../interface \
	 $(LDFLAGS)


lib: WMProxy.h $(SOAPH) $(SOAPCPP)
	$(CPP) $(CFLAGS) -I$(BOOST)/include \
	-shared -o libwmproxy_api_cpp.so wmproxy_api.cpp soapC.cpp soapClient.cpp $(SOAPCPP) $(LIBS) $(LDFLAGS)

clean:		
		rm -f *.o soapH.h soapStub.h soapC.cpp soapC.c soapClient.cpp soapClient.c \
			soapServer.cpp soapServer.c soap*Proxy.h
distclean:	
		rm -f *.o *.xsd *.xml *.nsmap *.log soapH.h soapStub.h soapC.cpp soapC.c soapClient.cpp \
			soapClient.c soapServer.cpp soapServer.c soap*Proxy.h
