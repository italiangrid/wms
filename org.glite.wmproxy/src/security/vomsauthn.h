/* Copyright (c) Members of the EGEE Collaboration. 2004.
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
limitations under the License. */

#ifndef GLITE_WMS_WMPROXY_WMPVOMSAUTHZ_H
#define GLITE_WMS_WMPROXY_WMPVOMSAUTHZ_H

#include "boost/shared_ptr.hpp"
// edglog /usr/include/unistd.h compilation problem workaround:
// declaration of `char* crypt(const char*, const char*) throw ()'
// throws different exceptions
#if defined __GLIBC__ && __GLIBC__ == 2 && __GLIBC_MINOR__ == 3
#include <sys/cdefs.h>
#undef __THROW
#define __THROW
#endif

#include "server/responsestruct.h"
#include "voms/voms_api.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace security {

long getProxyTimeLeft(std::string const& pxfile);
time_t ASN1_UTCTIME_get(ASN1_UTCTIME const* s);

class VOMSAuthN
{
public:
   VOMSAuthN(std::string const& proxypath);
   ~VOMSAuthN() { }
   bool hasVOMSExtension();
   std::string getVO();
   std::string getDN();
   std::string getDefaultFQAN();
   std::vector<std::string> getFQANs();
   VOProxyInfoStructType* getDefaultVOProxyInfo();
   ProxyInfoStructType* getProxyInfo();
private:
   boost::shared_ptr<X509> cert_;
   boost::shared_ptr<vomsdata> data_;
   boost::shared_ptr<voms> defaultvoms_;
};

}}}}

#endif // GLITE_WMS_WMPROXY_WMPVOMSAUTHZ_H
