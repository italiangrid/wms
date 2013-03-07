C=g++
COFLAGS=-O2
CWFLAGS=
ROOT=../../../
REPOSITORY=$(ROOT)/repository
STAGE=$(ROOT)/stage
SRC_DIR=$(ROOT)/org.glite.wms.wmproxy/src/authorizer

OBJFLAGS    = -g -Wall
SRC=testgaclmanager.cpp
OBJ=test-gacl-manager

CFLAGS= $(CWFLAGS) $(COFLAGS) $(CIFLAGS)

INCLUDE=-I$(SRC_DIR) -I/usr/include/openssl/ -I/usr/kerberos/include \
	-I$(ROOT)/org.glite.wms-utils.exception/interface 

LIBS=-lz -lssl$(FLAVOR_EXT) -lcrypto_gcc32pthr -lxml2 -lm -lglite_wmsutils_exception \
	-lgridsite  -lmcnab -lgacl-manager \
	-L $(SRC_DIR) \
	-L ../lib \
	-L$(STAGE)/lib \
	-L/usr/lib  -L$(REPOSITORY)/globus/2.4.3-VDT-1.2.2/rhel30_gcc32/lib


all:  test


test:
	$(C) $(OBJFLAGS) \
	-o $(OBJ)\
	$(SRC) \
	$(INCLUDE) \
	$(LIBS)
	




