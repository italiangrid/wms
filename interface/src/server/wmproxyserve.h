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
// File: wmproxyserve.h
// Author: Giuseppe Avellino <egee@datamat.it>
//

#ifndef GLITE_WMS_WMPROXY_WMPROXYSERVE_H
#define GLITE_WMS_WMPROXY_WMPROXYSERVE_H

#include "soapH.h"
#include "soapWMProxyObject.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace server {

extern long servedrequestcount_global;

class WMProxyServe: public WMProxyService
{
public:
   SOAP_FMAC5 int SOAP_FMAC6 wmproxy_soap_serve(struct soap *soap);
   virtual int serve() {
      return wmproxy_soap_serve(this);
   };
};

}}}}
#endif // GLITE_WMS_WMPROXY_WMPROXYSERVE_H
