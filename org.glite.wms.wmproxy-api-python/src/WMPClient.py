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


#
# This file defines two sub-classes of SOAPpy classes HTTPTransport and SOAPProxy
# in order to give the possibility to provide cert key and cert file argument to httplib.HTTPS
# method (this allows proxy info to be passes to the server)
#

from __future__ import nested_scopes

#import xml.sax
import urllib
from types import *
import re
import base64
import httplib


# SOAPpy modules
from SOAPpy.Client import *
from SOAPpy.version import __version__
from SOAPpy.Errors import *
from SOAPpy.Config      import Config
from SOAPpy.Parser      import parseSOAPRPC
from SOAPpy.SOAPBuilder import buildSOAP
from SOAPpy.Utilities   import *
from SOAPpy.Types       import faultType, simplify

from SOAPpy.Client	 import SOAPAddress, HTTPTransport, SOAPProxy


key=None
cert=None


#
# WMPClient
#

def SOAPUserAgent():
    return "SOAPpy " + __version__ + " (pywebsvcs.sf.net)"

# Changed from SOAPpy Client
# New call method takes two more argument key_file and cert_file
class WMPHTTPTransport(HTTPTransport):

    auth = True

    userCert = ""

    def setAuth(self, authenticate):
        self.auth = authenticate

    def setUserCert(self, cert):
        self.userCert = cert

    def call(self, addr, data, namespace, soapaction = None, key_file = None, cert_file = None, encoding = None,
        http_proxy = None, config = Config):
        if not isinstance(addr, SOAPAddress):
            addr = SOAPAddress(addr, config)
        # Build a request
        if http_proxy:
            real_addr = http_proxy
            real_path = addr.proto + "://" + addr.host + addr.path
        else:
            real_addr = addr.host
            real_path = addr.path

        if addr.proto == 'httpg':
            from pyGlobus.io import GSIHTTP
            r = GSIHTTP(real_addr, tcpAttr = config.tcpAttr)
        elif addr.proto == 'https' and self.auth == True:
            # HTTPS class extended: certificates verification added
            from WMPConnection import WMPHTTPS
            r = WMPHTTPS(real_addr,  key_file = key_file, cert_file = cert_file, user_cert = self.userCert)
        elif addr.proto == 'https' and self.auth == False:
            # certificates verification will not be performed
            r = httplib.HTTPS(real_addr,  key_file = key_file, cert_file = cert_file)
        else:
            r = httplib.HTTP(real_addr)

        r.putrequest("POST", real_path)

        r.putheader("Host", addr.host)
        r.putheader("User-agent", SOAPUserAgent())
        t = 'text/xml';
        if encoding != None:
            t += '; charset="%s"' % encoding
        r.putheader("Content-type", t)
        r.putheader("Content-length", str(len(data)))

        # if user is not a user:passwd format
        #    we'll receive a failure from the server. . .I guess (??)
        if addr.user != None:
            val = base64.encodestring(addr.user)
            r.putheader('Authorization','Basic ' + val.replace('\012',''))

        # This fixes sending either "" or "None"
        if soapaction == None or len(soapaction) == 0:
            r.putheader("SOAPAction", "")
        else:
            r.putheader("SOAPAction", '"%s"' % soapaction)

        if config.dumpHeadersOut:
            s = 'Outgoing HTTP headers'
            debugHeader(s)
            print "POST %s %s" % (real_path, r._http_vsn_str)
            print "Host:", addr.host
            print "User-agent: SOAPpy " + __version__ + " (http://pywebsvcs.sf.net)"
            print "Content-type:", t
            print "Content-length:", len(data)
            print 'SOAPAction: "%s"' % soapaction
            debugFooter(s)

        r.endheaders()

        if config.dumpSOAPOut:
            s = 'Outgoing SOAP'
            debugHeader(s)
            print data,
            if data[-1] != '\n':
                print
            debugFooter(s)

        # send the payload
        r.send(data)

        # read response line
        code, msg, headers = r.getreply()

        if headers:
            content_type = headers.get("content-type","text/xml")
            content_length = headers.get("Content-length")
        else:
            content_type=None
            content_length=None

        # work around OC4J bug which does '<len>, <len>' for some reaason
        if content_length:
            comma=content_length.find(',')
            if comma>0:
                content_length = content_length[:comma]

        # attempt to extract integer message size
        try:
            message_len = int(content_length)
        except:
            message_len = -1

        if message_len < 0:
            # Content-Length missing or invalid; just read the whole socket
            # This won't work with HTTP/1.1 chunked encoding
            data = r.getfile().read()
            message_len = len(data)
        else:
            data = r.getfile().read(message_len)

        if(config.debug):
            print "code=",code
            print "msg=", msg
            print "headers=", headers
            print "content-type=", content_type
            print "data=", data

        if config.dumpHeadersIn:
            s = 'Incoming HTTP headers'
            debugHeader(s)
            if headers.headers:
                print "HTTP/1.? %d %s" % (code, msg)
                print "\n".join(map (lambda x: x.strip(), headers.headers))
            else:
                print "HTTP/0.9 %d %s" % (code, msg)
            debugFooter(s)

        def startswith(string, val):
            return string[0:len(val)] == val

        if code == 500 and not \
               ( startswith(content_type, "text/xml") and message_len > 0 ):
            raise HTTPError(code, msg)

        if config.dumpSOAPIn:
            s = 'Incoming SOAP'
            debugHeader(s)
            print data,
            if (len(data)>0) and (data[-1] != '\n'):
                print
            debugFooter(s)

        if code not in (200, 500):
            raise HTTPError(code, msg)


        # get the new namespace
        if namespace is None:
            new_ns = None
        else:
            new_ns = self.getNS(namespace, data)

        # return response payload
        return data, new_ns

#
# WMPSOAPProxy
#
class WMPSOAPProxy(SOAPProxy):
    def __init__(self, proxy, namespace = None, soapaction = None, key_file = None, cert_file = None,
                 header = None, methodattrs = None, transport = WMPHTTPTransport,
                 encoding = 'UTF-8', throw_faults = 1, unwrap_results = None,
                 http_proxy=None, config = Config, noroot = 0,
                 simplify_objects=None, auth = True, certificates = ""):

        # Test the encoding, raising an exception if it's not known
        if encoding != None:
            ''.encode(encoding)

        # get default values for unwrap_results and simplify_objects
        # from config
        if unwrap_results is None:
            self.unwrap_results=config.unwrap_results
        else:
            self.unwrap_results=unwrap_results

        if simplify_objects is None:
            self.simplify_objects=config.simplify_objects
        else:
            self.simplify_objects=simplify_objects

        self.proxy          = SOAPAddress(proxy, config)
        self.namespace      = namespace
        self.soapaction     = soapaction
        self.header         = header
        self.methodattrs    = methodattrs
        self.transport      = transport()
        self.transport.setAuth(auth)
	self.transport.setUserCert(certificates)
        self.encoding       = encoding
        self.throw_faults   = throw_faults
        self.http_proxy     = http_proxy
        self.config         = config
        self.noroot         = noroot

	# New arguments
	self.key_file	    = key_file
	self.cert_file	    = cert_file

        # GSI Additions
        if hasattr(config, "channel_mode") and \
               hasattr(config, "delegation_mode"):
            self.channel_mode = config.channel_mode
            self.delegation_mode = config.delegation_mode
        #end GSI Additions

    def __call(self, name, args, kw, ns = None, sa = None, hd = None,
        ma = None):
        ns = ns or self.namespace
        ma = ma or self.methodattrs

        if sa: # Get soapaction
            if type(sa) == TupleType:
                sa = sa[0]
        else:
            if self.soapaction:
                sa = self.soapaction
            else:
                sa = name

        if hd: # Get header
            if type(hd) == TupleType:
                hd = hd[0]
        else:
            hd = self.header

        hd = hd or self.header

        if ma: # Get methodattrs
            if type(ma) == TupleType: ma = ma[0]
        else:
            ma = self.methodattrs
        ma = ma or self.methodattrs

        m = buildSOAP(args = args, kw = kw, method = name, namespace = ns,
            header = hd, methodattrs = ma, encoding = self.encoding,
            config = self.config, noroot = self.noroot)


        call_retry = 0
        try:
            r, self.namespace = self.transport.call(self.proxy, m, ns, sa, key_file = self.key_file, cert_file = self.cert_file,
                                                    encoding = self.encoding,
                                                    http_proxy = self.http_proxy,
                                                    config = self.config)

        except Exception, ex:
            #
            # Call failed.
            #
            # See if we have a fault handling vector installed in our
            # config. If we do, invoke it. If it returns a true value,
            # retry the call.
            #
            # In any circumstance other than the fault handler returning
            # true, reraise the exception. This keeps the semantics of this
            # code the same as without the faultHandler code.
            #

            if hasattr(self.config, "faultHandler"):
                if callable(self.config.faultHandler):
                    call_retry = self.config.faultHandler(self.proxy, ex)
                    if not call_retry:
                        raise
                else:
                    raise
            else:
                raise

        if call_retry:
            r, self.namespace = self.transport.call(self.proxy, m, ns, sa, key_file = self.key_file, cert_file = self.cert_file,
                                                    encoding = self.encoding,
                                                    http_proxy = self.http_proxy,
                                                    config = self.config)


        p, attrs = parseSOAPRPC(r, attrs = 1)

        try:
            throw_struct = self.throw_faults and \
                isinstance (p, faultType)
        except:
            throw_struct = 0

        if throw_struct:
            if Config.debug:
                print p
            raise p

        # If unwrap_results=1 and there is only element in the struct,
        # SOAPProxy will assume that this element is the result
        # and return it rather than the struct containing it.
        # Otherwise SOAPproxy will return the struct with all the
        # elements as attributes.
        if self.unwrap_results:
            try:
                count = 0
                for i in p.__dict__.keys():
                    if i[0] != "_":  # don't count the private stuff
                        count += 1
                        t = getattr(p, i)
                if count == 1: # Only one piece of data, bubble it up
                    p = t
            except:
                pass

        # Automatically simplfy SOAP complex types into the
        # corresponding python types. (structType --> dict,
        # arrayType --> array, etc.)
        if self.simplify_objects:
            p = simplify(p)

        if self.config.returnAllAttrs:
            return p, attrs
        return p

    def _callWithBody(self, body):
        return self.__call(None, body, {})

    def __getattr__(self, name):  # hook to catch method calls
        if name == '__del__':
            raise AttributeError, name
        return self.__Method(self.__call, name, config = self.config)




    # To handle attribute wierdness
    class __Method:
        # Some magic to bind a SOAP method to an RPC server.
        # Supports "nested" methods (e.g. examples.getStateName) -- concept
        # borrowed from xmlrpc/soaplib -- www.pythonware.com
        # Altered (improved?) to let you inline namespaces on a per call
        # basis ala SOAP::LITE -- www.soaplite.com

        def __init__(self, call, name, ns = None, sa = None, hd = None,
            ma = None, config = Config):

            self.__call 	= call
            self.__name 	= name
            self.__ns   	= ns
            self.__sa   	= sa
            self.__hd   	= hd
            self.__ma           = ma
            self.__config       = config
            return

        def __call__(self, *args, **kw):
            if self.__name[0] == "_":
                if self.__name in ["__repr__","__str__"]:
                    return self.__repr__()
                else:
                    return self.__f_call(*args, **kw)
            else:
                return self.__r_call(*args, **kw)
                        
        def __getattr__(self, name):
            if name == '__del__':
                raise AttributeError, name
            if self.__name[0] == "_":
                # Don't nest method if it is a directive
                return self.__class__(self.__call, name, self.__ns,
                    self.__sa, self.__hd, self.__ma)

            return self.__class__(self.__call, "%s.%s" % (self.__name, name),
                self.__ns, self.__sa, self.__hd, self.__ma)

        def __f_call(self, *args, **kw):
            if self.__name == "_ns": self.__ns = args
            elif self.__name == "_sa": self.__sa = args
            elif self.__name == "_hd": self.__hd = args
            elif self.__name == "_ma": self.__ma = args
            return self

        def __r_call(self, *args, **kw):
            return self.__call(self.__name, args, kw, self.__ns, self.__sa,
                self.__hd, self.__ma)

        def __repr__(self):
            return "<%s at %d>" % (self.__class__, id(self))
