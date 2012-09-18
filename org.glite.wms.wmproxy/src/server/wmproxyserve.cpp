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
// Author: Giuseppe Avellino <egee@datamat.it>
//

#include <stdsoap2.h>
#include <fcgi_stdio.h> 
#include "utilities/logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "wmproxyserve.h"
#include "configuration.h"
#include "signalhandler.h"

extern WMProxyConfiguration conf;

namespace glite {
namespace wms {
namespace wmproxy {
namespace server {

long servedrequestcount_global;

SOAP_FMAC5 int SOAP_FMAC6
WMProxyServe::wmproxy_soap_serve(struct soap *soap)
{
   while (0 == handled_signal_recv) {

      // Checking for per instance served requests count. If the maximum
      // value is reached, the server instance is terminated
      long plafond;
      char *env_plafond = getenv("GLITE_WMS_WMPROXY_MAX_SERVED_REQUESTS");
      if (env_plafond) {
         plafond = atol(env_plafond);
      } else if (conf.getMaxServedRequests()) {
         plafond = conf.getMaxServedRequests();
      }
      if (plafond <= 0) {
         plafond = 50;
      }
      if (servedrequestcount_global >= plafond) {
         edglog(info) << "-------- Exiting Server Instance -------" << std::endl;
         edglog(info) << "Maximum core request count reached: "<< plafond << std::endl;
         FCGI_Finish();
         break;
      }
      if (FCGI_Accept() >= 0 ) {
         soap_begin(soap);

         if (soap_begin_recv(soap)) {
            if (soap->error < SOAP_STOP) {
               soap_send_fault(soap);
            }
            soap_closesock(soap);
            continue;
         }

         if (soap_envelope_begin_in(soap)
               || soap_recv_header(soap)
               || soap_body_begin_in(soap)
               || soap_serve_request(soap)
               || (soap->fserveloop && soap->fserveloop(soap))) {

            soap_send_fault(soap);
         }
      } else {
         soap->error = SOAP_EOF;
         soap_send_fault(soap);
      }

      if (handled_signal_recv > 0) {
         edglog(info) << "-------- Exiting Server Instance -------" << std::endl;
         edglog(info) << "Signal code received: "<< handled_signal_recv << std::endl;
         break;
      }
   } // while

   soap_destroy(soap);
   soap_end(soap);
   //soap_free(soap);

   return SOAP_OK;
}

}}}}
