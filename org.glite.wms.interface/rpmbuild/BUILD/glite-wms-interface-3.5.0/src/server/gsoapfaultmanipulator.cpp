/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//
// File: gsoapfaultmanipulator.cpp
// Author: Giuseppe Avellino <egee@datamat.it>
//

// Boost
#include <boost/lexical_cast.hpp>

// gSOAP
#include "soapH.h"

#include "utilities/wmpexception_codes.h"

//Logger
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "utilities/logging.h"

namespace logger       = glite::wms::common::logger;
namespace wmputilities = glite::wms::wmproxy::utilities; //Exception codes

using namespace std;

// To remove
vector<string> *
convertStackVector(vector<string> stack)
{
   vector<string> *returnVector = new vector<string>(0);
   if (&stack) {
      string element;
      for (unsigned int i = 0; i < stack.size(); i++) {
         element = &(stack[i][0]);
         returnVector->push_back(element);
      }
   }
   return returnVector;
}

int
getServiceFaultType(int code)
{
   switch (code) {
   case wmputilities::WMS_AUTHENTICATION_ERROR: // AuthenticationFault
      return SOAP_TYPE_ns1__AuthenticationFaultType;
      break;

   case wmputilities::WMS_AUTHORIZATION_ERROR: // AuthorizationFault

   case wmputilities::WMS_PROXY_ERROR:
   case wmputilities::WMS_PROXY_EXPIRED:
   case wmputilities::WMS_DELEGATION_ERROR:
   case wmputilities::WMS_NOT_A_VOMS_PROXY:
   case wmputilities::WMS_PROXY_RENEWAL_FAILURE:

   case wmputilities::WMS_USERMAP_ERROR:

   case wmputilities::WMS_GACL_FILE:
   case wmputilities::WMS_GACL_ERROR:
   case wmputilities::WMS_GACL_ITEM_NOT_FOUND:
      return SOAP_TYPE_ns1__AuthorizationFaultType;
      break;

   case wmputilities::WMS_JOB_NOT_DONE: // OperationNotAllowedFault
   case wmputilities::WMS_OPERATION_NOT_ALLOWED: // Generated inside wmproxy code
      return SOAP_TYPE_ns1__OperationNotAllowedFaultType;
      break;

   case wmputilities::WMS_JDL_PARSING: // InvalidArgumentFault
   case wmputilities::WMS_INVALID_ARGUMENT:
      return SOAP_TYPE_ns1__InvalidArgumentFaultType;
      break;

   case wmputilities::WMS_JOB_NOT_FOUND: // JobUnknownFault
      return SOAP_TYPE_ns1__JobUnknownFaultType;
      break;

   case wmputilities::WMS_NO_SUITABLE_RESOURCE: // NoSuitableResourcesFault
   case wmputilities::WMS_MATCHMAKING: // ?
      return SOAP_TYPE_ns1__NoSuitableResourcesFaultType;
      break;

   case wmputilities::WMS_NOT_ENOUGH_QUOTA: // GetQuotaManagementFault
   case wmputilities::WMS_NOT_ENOUGH_SPACE: // ?
      return SOAP_TYPE_ns1__GetQuotaManagementFaultType;
      break;

   case wmputilities::WMS_SERVER_OVERLOADED:
      return SOAP_TYPE_ns1__ServerOverloadedFaultType;
      break;

   default:
      return SOAP_TYPE_ns1__GenericFaultType;
      break;

      // WMS_CONNECTION_ERROR -> soap server
   }
}

void *
initializeStackPointer(int code)
{
   void *sp = NULL;
   switch(getServiceFaultType(code)) {
   case SOAP_TYPE_ns1__AuthenticationFaultType:
      sp = new ns1__AuthenticationFaultType;
      break;
   case SOAP_TYPE_ns1__AuthorizationFaultType:
      sp = new ns1__AuthorizationFaultType;
      break;
   case SOAP_TYPE_ns1__InvalidArgumentFaultType:
      sp = new ns1__InvalidArgumentFaultType;
      break;
   case SOAP_TYPE_ns1__GetQuotaManagementFaultType:
      sp = new ns1__GetQuotaManagementFaultType;
      break;
   case SOAP_TYPE_ns1__NoSuitableResourcesFaultType:
      sp = new ns1__NoSuitableResourcesFaultType;
      break;
   case SOAP_TYPE_ns1__GenericFaultType:
      sp = new ns1__GenericFaultType;
      break;
   case SOAP_TYPE_ns1__JobUnknownFaultType:
      sp = new ns1__JobUnknownFaultType;
      break;
   case SOAP_TYPE_ns1__OperationNotAllowedFaultType:
      sp = new ns1__OperationNotAllowedFaultType;
      break;
   case SOAP_TYPE_ns1__ServerOverloadedFaultType:
      sp = new ns1__ServerOverloadedFaultType;
      break;

   default:
      sp = new ns1__GenericFaultType;
      break;
   }
   return sp;
}

void
setFaultDetails(struct soap *soap, int type, void *sp)
{
   if (soap->version == 2) {
      soap->fault->SOAP_ENV__Detail = (struct SOAP_ENV__Detail*)
                                      soap_malloc(soap, sizeof(struct SOAP_ENV__Detail));
      soap->fault->SOAP_ENV__Detail->__type = type; // stack type
      soap->fault->SOAP_ENV__Detail->fault = sp; // point to stack
      soap->fault->SOAP_ENV__Detail->__any = NULL; // no other XML data
   } else {
      soap->fault->detail = (struct SOAP_ENV__Detail*)
                            soap_malloc(soap, sizeof(struct SOAP_ENV__Detail));
      soap->fault->detail->__type = type; // stack type
      soap->fault->detail->fault = sp; // point to stack
      soap->fault->detail->__any = NULL; // no other XML data
   }
}

void
setSOAPFault(struct soap *soap, int code, const string& method_name, time_t time_stamp,
             int error_code, const string& description, vector<string> stack)
{

   edglog(info) <<"------------------------------- Fault description "
                 "--------------------------------" <<endl;
   edglog(info)<<"Method: "<<method_name<<endl;
   edglog(info)<<"Code: "<<boost::lexical_cast<std::string>(code)<<endl;
   edglog(info)<<"Description: "<<description<<endl;
   edglog(debug)<<"Stack: "<<endl;
   std::string error_stack;
   for (unsigned int i = 0; i < stack.size(); i++) {
      error_stack += std::string(stack[i]) + '\n';
   }
   edglog(debug)<< error_stack << endl;
   edglog(info) <<"----------------------------------------"
                 "------------------------------------------" <<endl;

   if (code == SOAP_TYPE__delegationns__DelegationException) {
      // Generating a fault
      _delegationns__DelegationException *sp = new _delegationns__DelegationException;

      // Filling fault field
      sp->msg = new string(description);

      // Sending fault
      soap_receiver_fault(soap, description.c_str(), NULL);
      // crashes in gSOAP 2.7.13
      // setFaultDetails(soap, SOAP_TYPE__delegationns__DelegationException, sp);

   } else {
      // Generating a fault
      ns1__BaseFaultType *sp = (ns1__BaseFaultType*)initializeStackPointer(code);

      // Filling fault fields
      sp->methodName = method_name;
      sp->Timestamp = time_stamp;
      sp->ErrorCode = new string(boost::lexical_cast<std::string>(error_code));
      sp->Description = new string(description);
      sp->FaultCause = *convertStackVector(stack);

      // Sending fault
      soap_receiver_fault(soap, description.c_str(), NULL);
      // crashes in gSOAP 2.7.13
      //setFaultDetails(soap, getServiceFaultType(code), sp);
   }
}

void
setSOAPFault(struct soap *soap, int code, const string& method_name, time_t time_stamp,
             int error_code, const string& description)
{
   setSOAPFault(soap, code, method_name, time_stamp, error_code, description,
                vector<string>());
}
