/* soapDelegationSoapBindingProxy.h
   Generated by gSOAP 2.7.16 from wm.h
   Copyright(C) 2000-2010, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/

#ifndef soapDelegationSoapBindingProxy_H
#define soapDelegationSoapBindingProxy_H
#include "soapH.h"
class DelegationSoapBinding
{   public:
	/// Runtime engine context allocated in constructor
	struct soap *soap;
	/// Endpoint URL of service 'DelegationSoapBinding' (change as needed)
	const char *endpoint;
	/// Constructor allocates soap engine context, sets default endpoint URL, and sets namespace mapping table
	DelegationSoapBinding()
	{ soap = soap_new(); endpoint = "https://localhost:8443/glite-security-delegation"; if (soap && !soap->namespaces) { static const struct Namespace namespaces[] = 
{
	{"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope", NULL},
	{"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"jsdlposix", "http://schemas.ggf.org/jsdl/2005/11/jsdl-posix", NULL, NULL},
	{"jsdl", "http://schemas.ggf.org/jsdl/2005/11/jsdl", NULL, NULL},
	{"delegation1", "http://www.gridsite.org/namespaces/delegation-1", NULL, NULL},
	{"delegationns", "http://www.gridsite.org/namespaces/delegation-2", NULL, NULL},
	{"ns1", "http://glite.org/wms/wmproxy", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};
	soap->namespaces = namespaces; } };
	/// Destructor frees deserialized data and soap engine context
	virtual ~DelegationSoapBinding() { if (soap) { soap_destroy(soap); soap_end(soap); soap_free(soap); } };
	/// Invoke 'getVersion' of service 'DelegationSoapBinding' and return error code (or SOAP_OK)
	virtual int delegationns__getVersion(struct delegationns__getVersionResponse &_param_3) { return soap ? soap_call_delegationns__getVersion(soap, endpoint, NULL, _param_3) : SOAP_EOM; };
	/// Invoke 'getInterfaceVersion' of service 'DelegationSoapBinding' and return error code (or SOAP_OK)
	virtual int delegationns__getInterfaceVersion(struct delegationns__getInterfaceVersionResponse &_param_4) { return soap ? soap_call_delegationns__getInterfaceVersion(soap, endpoint, NULL, _param_4) : SOAP_EOM; };
	/// Invoke 'getServiceMetadata' of service 'DelegationSoapBinding' and return error code (or SOAP_OK)
	virtual int delegationns__getServiceMetadata(std::string _key, struct delegationns__getServiceMetadataResponse &_param_5) { return soap ? soap_call_delegationns__getServiceMetadata(soap, endpoint, NULL, _key, _param_5) : SOAP_EOM; };
	/// Invoke 'getProxyReq' of service 'DelegationSoapBinding' and return error code (or SOAP_OK)
	virtual int delegationns__getProxyReq(std::string _delegationID, struct delegationns__getProxyReqResponse &_param_6) { return soap ? soap_call_delegationns__getProxyReq(soap, endpoint, NULL, _delegationID, _param_6) : SOAP_EOM; };
	/// Invoke 'getNewProxyReq' of service 'DelegationSoapBinding' and return error code (or SOAP_OK)
	virtual int delegationns__getNewProxyReq(struct delegationns__getNewProxyReqResponse &_param_7) { return soap ? soap_call_delegationns__getNewProxyReq(soap, endpoint, NULL, _param_7) : SOAP_EOM; };
	/// Invoke 'renewProxyReq' of service 'DelegationSoapBinding' and return error code (or SOAP_OK)
	virtual int delegationns__renewProxyReq(std::string _delegationID, struct delegationns__renewProxyReqResponse &_param_8) { return soap ? soap_call_delegationns__renewProxyReq(soap, endpoint, NULL, _delegationID, _param_8) : SOAP_EOM; };
	/// Invoke 'putProxy' of service 'DelegationSoapBinding' and return error code (or SOAP_OK)
	virtual int delegationns__putProxy(std::string _delegationID, std::string _proxy, struct delegationns__putProxyResponse &_param_9) { return soap ? soap_call_delegationns__putProxy(soap, endpoint, NULL, _delegationID, _proxy, _param_9) : SOAP_EOM; };
	/// Invoke 'getTerminationTime' of service 'DelegationSoapBinding' and return error code (or SOAP_OK)
	virtual int delegationns__getTerminationTime(std::string _delegationID, struct delegationns__getTerminationTimeResponse &_param_10) { return soap ? soap_call_delegationns__getTerminationTime(soap, endpoint, NULL, _delegationID, _param_10) : SOAP_EOM; };
	/// Invoke 'destroy' of service 'DelegationSoapBinding' and return error code (or SOAP_OK)
	virtual int delegationns__destroy(std::string _delegationID, struct delegationns__destroyResponse &_param_11) { return soap ? soap_call_delegationns__destroy(soap, endpoint, NULL, _delegationID, _param_11) : SOAP_EOM; };
};
#endif