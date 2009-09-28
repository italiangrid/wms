// File: ssl_utils.cpp
// Author: Salvatore Monforte

// $Id$

#include <string>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <boost/shared_ptr.hpp>
#include <ctime>

namespace {

boost::shared_ptr< ::X509_REQ>
generate_request(
  boost::shared_ptr< ::X509> cert,
  ::EVP_PKEY *pkey
)
{
  boost::shared_ptr< ::X509_REQ> req(
    ::X509_REQ_new(), ::X509_REQ_free
  );
  
   boost::shared_ptr< X509_NAME> name(
    X509_NAME_dup(X509_get_subject_name(cert.get())),
    X509_NAME_free
  );

  boost::shared_ptr<X509_NAME_ENTRY> ne(
    X509_NAME_ENTRY_create_by_NID(
      0,NID_commonName,V_ASN1_APP_CHOOSE, (unsigned char *)"proxy", -1
    ),
    X509_NAME_ENTRY_free
  );
    
  X509_NAME_add_entry(
    name.get(), ne.get(), X509_NAME_entry_count(name.get()),  0
  );
 
  X509_REQ_set_subject_name(req.get(), name.get());
  X509_REQ_set_pubkey(req.get(),pkey);

  if (!X509_REQ_sign(req.get(),pkey,EVP_md5())) {
    return boost::shared_ptr< ::X509_REQ>();
  }
  return req;
}

boost::shared_ptr< ::X509>
proxy_sign(
 boost::shared_ptr< ::X509_REQ> req,
 boost::shared_ptr< ::X509> cert,
 boost::shared_ptr< ::EVP_PKEY> pkey,
 time_t seconds
)
{
  if ( !::X509_REQ_verify(req.get(), ::X509_REQ_get_pubkey(req.get())) ) {
    return boost::shared_ptr< ::X509>();
  }

  boost::shared_ptr< ::X509> new_cert(
   ::X509_new(), ::X509_free
  );
 
  boost::shared_ptr< X509_NAME> name(
    X509_NAME_dup(X509_REQ_get_subject_name(req.get())),
    X509_NAME_free
  );
    
  if (!X509_set_subject_name(new_cert.get(), name.get())
    || !X509_set_issuer_name(new_cert.get(), X509_get_subject_name(cert.get())))
  {
    return boost::shared_ptr< ::X509>();
  }

  new_cert->cert_info->serialNumber =
    ASN1_INTEGER_dup(X509_get_serialNumber(cert.get()));

  // Allow for a five minute clock skew here.
  ::X509_gmtime_adj(
    X509_get_notBefore(new_cert),-300
  );
  
  // Doesn't create a proxy longer than the user cert
  if ( ::ASN1_UTCTIME_cmp_time_t(
        X509_get_notAfter(cert), std::time(0)+seconds
       ) < 0
  ) {
    ::X509_set_notAfter(
      new_cert.get(), cert->cert_info->validity->notAfter
    );
  }
  else {
    ::X509_gmtime_adj(
      X509_get_notAfter(new_cert), seconds
    );
  }

  // transfer the public key from req to new cert
  X509_PUBKEY_free(new_cert->cert_info->key);
  new_cert->cert_info->key = req->req_info->pubkey;
  req->req_info->pubkey = 0;

  // set version 2 certificate
  new_cert->cert_info->version = ASN1_INTEGER_new();
  ASN1_INTEGER_set(new_cert->cert_info->version,2);

  if (!X509_sign(new_cert.get(), pkey.get(), EVP_md5()))
  {
    return boost::shared_ptr< ::X509>();
  }
  return new_cert;    
}
}

namespace sslutils
{

bool
proxy_expires_within(
  std::string const& x509_proxy, 
  time_t seconds
)
{
  boost::shared_ptr<std::FILE> fd(
    std::fopen(x509_proxy.c_str(), "r"), std::fclose
  );
  if (!fd) return true;
  
  boost::shared_ptr< ::X509> cert(
    ::PEM_read_X509( (std::FILE*)(fd.get()), 0, 0, 0), ::X509_free
  );
  if (!cert) return true;

  return 
    ASN1_UTCTIME_cmp_time_t(
      X509_get_notAfter(cert.get()), std::time(0)+seconds
    ) < 0;
}

bool
proxy_init(
  std::string const& certfile,
  std::string const& keyfile,
  std::string const& outfile,
  time_t seconds
)
{  
  boost::shared_ptr<std::FILE> fd(
    std::fopen(certfile.c_str(), "r"), std::fclose
  );
  if (!fd) return false;
  
  boost::shared_ptr< ::X509> cert(
    ::PEM_read_X509( (std::FILE*)(fd.get()), 0, 0, 0), ::X509_free
  );
  if (!cert) return false;

  ::EVP_PKEY *pkey(::EVP_PKEY_new());
  ::RSA* rsa(::RSA_generate_key( 512, RSA_F4, 0, 0));

  if (!EVP_PKEY_assign_RSA(pkey,rsa)) 
  {
    ::RSA_free(rsa);
    ::EVP_PKEY_free(pkey);
    return false;
  }

  boost::shared_ptr< ::X509_REQ> req = generate_request(cert, pkey);
  if (!req) return false;

  boost::shared_ptr<std::FILE> fd_key(
    std::fopen(keyfile.c_str(), "rb"), std::fclose
  );
  if (!fd_key) return false;
  
  boost::shared_ptr< ::EVP_PKEY> upkey(
    ::PEM_read_PrivateKey( (std::FILE*)(fd_key.get()), 0, 0, 0), 
    ::EVP_PKEY_free
  );
     
  boost::shared_ptr< ::X509> new_cert = proxy_sign(
    req, cert, upkey, seconds
  );
  if(!new_cert)  return false; 
    
  FILE* fdo = std::fopen(outfile.c_str(), "w");
  PEM_write_X509(fdo,new_cert.get());
  PEM_write_RSAPrivateKey(fdo, pkey->pkey.rsa, 0,0,0,0,0);
  PEM_write_X509(fdo,cert.get());
  std::fclose(fdo);
  return true;
}

} // namespace sslutils closure
