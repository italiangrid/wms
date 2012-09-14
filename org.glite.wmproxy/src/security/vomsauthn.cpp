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

#include <string>
#include <openssl/pem.h>
#include <openssl/ssl.h>

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
#include "utilities/logging.h"
#include "utilities/wmpexceptions.h"
#include "utilities/wmpexception_codes.h"
#include "vomsauthn.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace security {

char const* X509_VOMS_DIR = "X509_VOMS_DIR";
char const* X509_CERT_DIR = "X509_CERT_DIR";
char const* VOMS_DIR = "/etc/grid-security/vomsdir";
char const* CERT_DIR = "/etc/grid-security/certificates";

namespace logger = glite::wms::common::logger;
namespace wmputilities = glite::wms::wmproxy::utilities;

using namespace std;

namespace {

STACK_OF(X509)*
load_chain(char const* certfile)
{
   STACK_OF(X509_INFO) *sk=NULL;
   STACK_OF(X509) *stack=NULL, *ret=NULL;
   BIO *in=NULL;
   X509_INFO *xi;
   int first = 1;

   if(!(stack = sk_X509_new_null())) {
      edglog(severe)<<"Memory allocation failure"<<endl;
      BIO_free(in);
      sk_X509_INFO_free(sk);
   }

   if(!(in=BIO_new_file(certfile, "r"))) {
      edglog(severe)<<"Error opening the file: "<<string(certfile)<<endl;
      BIO_free(in);
      sk_X509_INFO_free(sk);
   }

   // This loads from a file, a stack of x509/crl/pkey sets
   if(!(sk=PEM_X509_INFO_read_bio(in,NULL,NULL,NULL))) {
      edglog(severe)<<"Error reading the file: "<<string(certfile)<<endl;
      BIO_free(in);
      sk_X509_INFO_free(sk);
   }

   // scan over it and pull out the certs
   while (sk_X509_INFO_num(sk)) {
      // skip first cert
      if (first) {
         first = 0;
         continue;
      }
      xi=sk_X509_INFO_shift(sk);
      if (xi->x509 != 0) {
         sk_X509_push(stack,xi->x509);
         xi->x509 = 0;
      }
      X509_INFO_free(xi);
   }
   if(!sk_X509_num(stack)) {
      edglog(severe)<<"No certificates in file: "<<string(certfile)<<endl;
      sk_X509_free(stack);
      BIO_free(in);
      sk_X509_INFO_free(sk);
   }
   BIO_free(in);
   ret = stack;
   return ret;
}

const long
convASN1Date(const std::string& date)
{
   char     *str;
   time_t    offset;
   time_t    newtime = 0;
   char      buff1[32];
   char     *p;
   int       i;
   struct tm tm;
   int       size = 0;
   ASN1_TIME *ctm = ASN1_TIME_new();
   ctm->data   = (unsigned char *)(date.data());
   ctm->length = date.size();
   switch (ctm->length) {
   case 10: {
      ctm->type = V_ASN1_UTCTIME;
      break;
   }
   case 15: {
      ctm->type = V_ASN1_GENERALIZEDTIME;
      break;
   }
   default: {
      ASN1_TIME_free(ctm);
      ctm = 0;
      break;
   }
   }
   if (ctm) {
      switch (ctm->type) {
      case V_ASN1_UTCTIME: {
         size = 10;
         break;
      }
      case V_ASN1_GENERALIZEDTIME: {
         size = 12;
         break;
      }
      }
      p = buff1;
      i = ctm->length;
      str = (char *)ctm->data;
      if ((i < 11) || (i > 17)) {
         newtime = 0;
      }
      memcpy(p,str,size);
      p += size;
      str += size;


      if ((*str == 'Z') || (*str == '-') || (*str == '+')) {
         *(p++)='0';
         *(p++)='0';
      } else {
         *(p++)= *(str++);
         *(p++)= *(str++);
      }
      *(p++)='Z';
      *(p++)='\0';
      if (*str == 'Z') {
         offset=0;
      } else {
         if ((*str != '+') && (str[5] != '-')) {
            newtime = 0;
         }
         offset=((str[1]-'0')*10+(str[2]-'0'))*60;
         offset+=(str[3]-'0')*10+(str[4]-'0');
         if (*str == '-') {
            offset=-offset;
         }
      }
      tm.tm_isdst = 0;
      int index = 0;
      if (ctm->type == V_ASN1_UTCTIME) {
         tm.tm_year  = (buff1[index++]-'0')*10;
         tm.tm_year += (buff1[index++]-'0');
      } else {
         tm.tm_year  = (buff1[index++]-'0')*1000;
         tm.tm_year += (buff1[index++]-'0')*100;
         tm.tm_year += (buff1[index++]-'0')*10;
         tm.tm_year += (buff1[index++]-'0');
      }
      if (tm.tm_year < 70) {
         tm.tm_year+=100;
      }

      if (tm.tm_year > 1900) {
         tm.tm_year -= 1900;
      }

      tm.tm_mon   = (buff1[index++]-'0')*10;
      tm.tm_mon  += (buff1[index++]-'0')-1;
      tm.tm_mday  = (buff1[index++]-'0')*10;
      tm.tm_mday += (buff1[index++]-'0');
      tm.tm_hour  = (buff1[index++]-'0')*10;
      tm.tm_hour += (buff1[index++]-'0');
      tm.tm_min   = (buff1[index++]-'0')*10;
      tm.tm_min  += (buff1[index++]-'0');
      tm.tm_sec   = (buff1[index++]-'0')*10;
      tm.tm_sec  += (buff1[index++]-'0');
      newtime = (mktime(&tm) + offset*60*60 - timezone);
   }
   return newtime;
}

} // anonymous namespace

long
getProxyTimeLeft(string const& pxfile)
{
   GLITE_STACK_TRY("getProxyTimeLeft");
   edglog_fn("WMPAuthorizer::getProxyTimeLeft");

   time_t timeleft = 0;
   X509 *x = NULL;
   BIO *in = NULL;
   in = BIO_new(BIO_s_file());
   if (in) {
      BIO_set_close(in, BIO_CLOSE);
      if (BIO_read_filename(in, pxfile.c_str() ) > 0) {
         x = PEM_read_bio_X509(in, NULL, 0, NULL);
         if (!x) {
            BIO_free(in);
            edglog(severe) << "PEM_read_bio_X509: proxy file " << pxfile <<
               " doesn't exist or has bad permissions" << endl;
            throw wmputilities::AuthorizationException(__FILE__, __LINE__,
                  "VOMSAuthN::getProxyTimeLeft", wmputilities::WMS_AUTHORIZATION_ERROR,
                  "Proxy file doesn't exist or has bad permissions");
         }
         timeleft = (ASN1_UTCTIME_get(X509_get_notAfter(x)) - time(NULL))
                    / 60;
         free(x);
      } else {
         BIO_free(in);
         edglog(error)<<"Unable to get the proxy time left"<<endl;
         throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
               "BIO_read_filename", wmputilities::WMS_PROXY_ERROR,
               "Unable to get the proxy time left");
      }
      BIO_free(in);
   } else {
      edglog(error)<<"Unable to get the proxy time left (BIO SSL error)"<<endl;
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "BIO_new", wmputilities::WMS_PROXY_ERROR,
            "Unable to get the proxy time left (BIO SSL error)");

   }
   return timeleft;

   GLITE_STACK_CATCH();
}

VOMSAuthN::VOMSAuthN(const string& proxypath)
   : data_(new vomsdata), defaultvoms_(new voms)
{
   GLITE_STACK_TRY("VOMSAuthN::VOMSAuthN(const string &proxypath)");

   edglog_fn("VOMSAuthN::VOMSAuthN(const string &proxypath)");
   edglog(debug)<<"Proxy path: " << proxypath << endl;

   char* envval = 0;
   char* vomsdir = 0;
   char* certdir = 0;
   if ((envval = getenv(X509_VOMS_DIR))) {
      vomsdir = envval;
   } else {
      vomsdir = const_cast<char*>(VOMS_DIR);
   }
   if ((envval = getenv(X509_CERT_DIR))) {
      certdir = envval;
   } else {
      certdir = const_cast<char*>(CERT_DIR);
   }
   *data_ = vomsdata(vomsdir, certdir);

   SSL_library_init();
   BIO* in = 0;
   STACK_OF(X509)* chain = 0;
   in = BIO_new(BIO_s_file());
   if (in) {
      if (BIO_read_filename(in, proxypath.c_str())) {
         ::X509* cert = PEM_read_bio_X509(in, 0, 0, 0);
         cert_.reset(cert, ::X509_free);
         if (!cert_) {
            BIO_free(in);
            throw wmputilities::AuthorizationException(__FILE__, __LINE__,
                  "VOMSAuthZ::VOMSAuthN", wmputilities::WMS_AUTHORIZATION_ERROR,
                  "Proxy file doesn't exist or has bad permissions");
         }

         // Verifing all flags except for VERIFY_DATE
         // Time left verification is done by a different method
         data_->SetVerificationType(
            verify_type(
               VERIFY_TARGET | VERIFY_SIGN | VERIFY_ORDER
               | VERIFY_CERTLIST | VERIFY_KEY | VERIFY_ID));
         if (!data_) {
            BIO_free(in);
            throw wmputilities::AuthorizationException(__FILE__, __LINE__,
                  "VOMSAuthZ::VOMSAuthN", wmputilities::WMS_AUTHORIZATION_ERROR,
                  data_->ErrorMessage());
         }
         chain = load_chain(proxypath.c_str());
         if (!data_->Retrieve(cert_.get(), chain, RECURSE_CHAIN)) {
            BIO_free(in);
            throw wmputilities::AuthorizationException(
               __FILE__, __LINE__,
               "VOMSAuthZ::VOMSAuthN",
               wmputilities::WMS_AUTHORIZATION_ERROR,
               data_->ErrorMessage());
         }
         if (!data_->DefaultData(*defaultvoms_)) { // allocates
            throw wmputilities::AuthorizationException(__FILE__, __LINE__,
                  "VOMSAuthN::VOMSAuthN", wmputilities::WMS_AUTHORIZATION_ERROR,
                  data_->ErrorMessage());
         }
         BIO_free(in);
      } else {
         BIO_free(in);
         edglog(severe)<<"Error in BIO_read_filename: Proxy file doesn't "
                       "exist or has bad permissions"<<endl;
         throw wmputilities::AuthorizationException(__FILE__, __LINE__,
               "VOMSAuthZ::VOMSAuthN", wmputilities::WMS_AUTHORIZATION_ERROR,
               "Proxy file doesn't exist or has bad permissions");
      }
   } else {
      edglog(severe) << "Error in BIO_new" << endl;
      throw wmputilities::AuthorizationException(__FILE__, __LINE__,
            "VOMSAuthZ::VOMSAuthN", wmputilities::WMS_AUTHORIZATION_ERROR,
            "Unable to get information from Proxy file");
   }

   GLITE_STACK_CATCH();
}

bool
VOMSAuthN::hasVOMSExtension()
{
   return data_ != 0;
}

std::vector<std::string> VOMSAuthN::getFQANs()
{
   if (defaultvoms_) {
      return defaultvoms_->fqan;
   } else {
      return std::vector<std::string>();
   }
}

std::string
VOMSAuthN::getDN()
{
   GLITE_STACK_TRY("getDN()");

   if (defaultvoms_) {
      return std::string(defaultvoms_->user);
   }
   return 0; // not a VOMS Proxy certificate

   GLITE_STACK_CATCH();
}

std::string
VOMSAuthN::getVO()
{
   if (defaultvoms_) {
      return defaultvoms_->voname;
   } else {
      return "";
   }
}

std::string
VOMSAuthN::getDefaultFQAN()
{
   GLITE_STACK_TRY("getDefaultFQAN()");

   if (defaultvoms_) {
      return defaultvoms_->fqan.front();
   } else {
      return ""; // not a VOMS Proxy certificate
   }

   GLITE_STACK_CATCH();
}

VOProxyInfoStructType *
VOMSAuthN::getDefaultVOProxyInfo()
{
   GLITE_STACK_TRY("getDefaultVOProxyInfo()");
   VOProxyInfoStructType* voproxyinfo = new VOProxyInfoStructType();
   if (data_ && defaultvoms_) {
      voproxyinfo->user = defaultvoms_->user;
      voproxyinfo->userCA = defaultvoms_->userca;
      voproxyinfo->server = defaultvoms_->server;
      voproxyinfo->serverCA = defaultvoms_->serverca;
      voproxyinfo->voName = defaultvoms_->voname;
      voproxyinfo->uri = defaultvoms_->uri;
      voproxyinfo->startTime = boost::lexical_cast<std::string>(
                                  convASN1Date(defaultvoms_->date1));
      voproxyinfo->endTime = boost::lexical_cast<std::string>(
                                convASN1Date(defaultvoms_->date2));
      voproxyinfo->attribute = defaultvoms_->fqan;
   }
   return voproxyinfo; // not a VOMS Proxy certificate

   GLITE_STACK_CATCH();
}

ProxyInfoStructType *
VOMSAuthN::getProxyInfo()
{
   static std::string const proxy_tag = "CN=";

   GLITE_STACK_TRY("getProxyInfo()");
   ProxyInfoStructType* proxyinfo = new ProxyInfoStructType();
   char* subject = X509_NAME_oneline(X509_get_subject_name(cert_.get()), 0, 0);

   if (subject) {
      string subjectstring = string(subject);
      if (subjectstring.find(proxy_tag) != string::npos) {
         proxyinfo->type = "proxy";
      } else {
         proxyinfo->type = "x509";
      }
   } else {
      proxyinfo->type = "unknown";
      vector<VOProxyInfoStructType*> voproxyinfovector;
      proxyinfo->vosInfo = voproxyinfovector;
   }

   proxyinfo->subject = string(subject);
   OPENSSL_free(subject);

   proxyinfo->issuer   = string(X509_NAME_oneline(X509_get_issuer_name(cert_.get()), 0, 0));
   // identity is the same as issuser
   proxyinfo->identity = proxyinfo->issuer;
   // getting strength
   int bits = -1;
   EVP_PKEY * key = X509_extract_key(cert_.get());
   bits = 8 * EVP_PKEY_size(key);
   if (key) {
      EVP_PKEY_free(key);
   }
   proxyinfo->strength = boost::lexical_cast<string>(bits);
   proxyinfo->startTime = boost::lexical_cast<std::string>(ASN1_UTCTIME_get(X509_get_notBefore(cert_)));
   proxyinfo->endTime = boost::lexical_cast<std::string>(ASN1_UTCTIME_get(X509_get_notAfter(cert_)));

   // getting voms info, if any
   if (data_) {
      proxyinfo->vosInfo.push_back(getDefaultVOProxyInfo());
   } else {
      edglog(warning)<<"The Proxy does not contain VOMS extension" << endl;
   }

   return proxyinfo;
   GLITE_STACK_CATCH();
}

time_t
ASN1_UTCTIME_get(const ASN1_UTCTIME *s)
{
   struct tm tm;
   int offset;

   memset(&tm, '\0', sizeof tm);
#define g2(p) (((p)[0]-'0')*10+(p)[1]-'0')
   tm.tm_year = g2(s->data);
   if (tm.tm_year < 50) {
      tm.tm_year += 100;
   }
   tm.tm_mon = g2(s->data + 2) - 1;
   tm.tm_mday = g2(s->data + 4);
   tm.tm_hour = g2(s->data + 6);
   tm.tm_min = g2(s->data + 8);
   tm.tm_sec = g2(s->data + 10);
   if (s->data[12] == 'Z') {
      offset = 0;
   } else {
      offset = g2(s->data + 13) * 60 + g2(s->data + 15);
      if (s->data[12] == '-') {
         offset = -offset;
      }
   }
#undef g2

   return timegm(&tm) - offset * 60;
}

}}}}
