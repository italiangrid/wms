################
#	Copyright (c) Members of the EGEE Collaboration. 2004.
#	See http://www.eu-egee.org/partners/ for details on the
#	copyright holders.
#
#	Licensed under the Apache License, Version 2.0 (the "License");
#	you may not use this file except in compliance with the License.
#	You may obtain a copy of the License at
#	 
#	     http://www.apache.org/licenses/LICENSE-2.0
#	 
#	Unless required by applicable law or agreed to in writing, software
#	distributed under the License is distributed on an "AS IS" BASIS,
#	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
#	either express or implied.
#	See the License for the specific language governing permissions and
#	limitations under the License.
################

import httplib
import socket
import time
import os
import glob
import sys

try:
    from OpenSSL import SSL
    from OpenSSL import crypto
except ImportError, err: 
    raise Exception ("Error: Unable to find pyOpenSSL module")


g_sHostName = "unknown"
g_cCtx = 0

def verifyCB( conn, cert, errnum, depth, ok):
    global g_sHostName
    if depth == 0 and ok == 1:
        sCN = cert.get_subject().commonName
        if g_sHostName == sCN or "host/%s" % g_sHostName == sCN:
            return 1
        else:
            return ok
    return ok

def initializeClientSSL( key_file = "", cert_file = "", cert = "" ):
    # Initialize context
    ctx = SSL.Context( SSL.SSLv23_METHOD )
    ctx.set_verify(SSL.VERIFY_PEER|SSL.VERIFY_FAIL_IF_NO_PEER_CERT, verifyCB) # Demand a certificate


    # Loads the user proxy
    if (key_file == "") and (cert_file == ""):
    	try:
                 sProxyLocation = os.environ['X509_USER_PROXY']
    	except:
                 sProxyLocation = '/tmp/x509up_u'+ repr(os.getuid())
    elif key_file != "":
	sProxyLocation = key_file;
    elif cert_file != "":
        sProxyLocation = cert_file;


    ctx.use_certificate_chain_file( sProxyLocation )
    ctx.use_privatekey_file(  sProxyLocation )

    CAdir = ""

    try:
        CAdir = os.environ['SSL_CERT_DIR']
    except:
        try:
            CAdir = os.environ['X509_CERT_DIR']
        except:
            CAdir = os.sep + os.path.join('etc', 'grid-security', 'certificates')

    # Looks for certificates inside the certificates directory
    if cert == "":
        path = os.path.join( CAdir ,'*')
        # Default certificates ends with one digit
        CApaths = glob.glob( path )
    else:
        path = os.path.join( CAdir , cert )
        # User extension for certificate file
        CApaths = glob.glob( path )

    cert_store = ctx.get_cert_store()
    for path in CApaths:
            try:
                 cert_store.add_cert(crypto.load_certificate(crypto.FILETYPE_PEM, open(path).read()))
            except:
                 pass

    ctx.set_session_id( "HTTPSConnection%s" % str( time.time() ) )
    
    if os.path.isfile("proxytemp"):
	os.remove("proxytemp")
    return ctx

class FakeSocket:

    def __getattr__(self, name):
        return getattr( self.sock, name )

    def __init__(self, sock):
        self.iCopies = 0
        self.sock = sock

    def close(self):
        if self.iCopies == 0:
            self.sock.shutdown()
            self.sock.close()
        else:
            self.iCopies -= 1

    def makefile(self, mode, bufsize=None):
        self.iCopies += 1
        return socket._fileobject( self.sock, mode, bufsize)


class WMPConnection( httplib.HTTPConnection):
    iMaximumRecursionLevel = 10

    def __init__( self, host, cert_file, key_file, bAllowCertificates = False, port = None, cert = "" ):
        global g_cCtx, g_sHostName
        httplib.HTTPConnection.__init__( self, host, port )
        if host.find(':') == -1:
            g_sHostName = socket.getfqdn( host )
        else:
            g_sHostName = socket.getfqdn( host[ : host.find(':') ] )
        if not g_cCtx:
            g_cCtx = initializeClientSSL(key_file, cert_file, cert )

    def close( self ):
        self.sock.close()

    def connect( self, recursionLevel = 0 ):
        global g_cCtx
        self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
        self.sock = FakeSocket( SSL.Connection( g_cCtx, self.socket )  )
        self.sock.connect( ( self.host, self.port ) )
        self.sock.set_connect_state()
        try:
            self.sock.do_handshake()
        except SSL.Error, v:
            raise Exception("\n".join( [ stError[2] for stError in v.args[0] ] ))


class WMPHTTPS(httplib.HTTPS):

   _connection_class = WMPConnection

   def __init__(self, host='', port=None, key_file=None, cert_file=None,
                     strict=None, user_cert = ""):

            self.key_file = key_file
            self.cert_file = cert_file
            self.user_cert = user_cert

            if port == 0:
                port = None
            self._setup(self._connection_class(host, port, key_file,
                                               cert_file, strict, user_cert))






 
