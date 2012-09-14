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
#include <sys/time.h>
#include <ldap.h>
#include <lber.h>

#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <exception>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

class LDAPException : public std::exception
{
  std::string m_error;
public:
  LDAPException(std::string const& error)
    : m_error(error)
  {
  }
  ~LDAPException() throw()
  {
  }
  virtual char const* what() const throw()
  {
    return m_error.c_str();
  }
};

int main(int ac, char* av[]) {

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "this help message")
    ("host,h", po::value<std::string>(), "LDAP server")
    ("port,p", po::value<std::string>()->default_value("2170"), "port on LDAP server")
    ("basedn,b", po::value<std::string>()->default_value("mds-vo-name=local,o=grid"), "base dn for search")
    ("filter,f", po::value<std::string>()->default_value("(objectclass=*)"), "RFC-2254 compliant LDAP search filter")
    ("attribute,a", po::value<std::vector<std::string> >()->composing(), "attribute description")
    ("timeout,t", po::value<int>(), "timeout for search")
    ("scope,s", po::value<size_t>()->default_value(2), "one of 0=base, 1=onelevel, 2=sub (search scope)")
  ;

  po::positional_options_description pd;
  pd.add("attribute", -1); 

  po::variables_map vm;
  po::store(po::command_line_parser(ac, av).
  options(desc).positional(pd).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }
 
  std::string host;
  if (vm.count("host")) {
    host.assign(vm["host"].as<std::string>());
  }
  LDAP* handle = 0 ;
  int result = ldap_initialize(&handle, std::string("ldap://"+host+":"+vm["port"].as<std::string>()).c_str());
  boost::shared_ptr<void> ldap_guard(
    static_cast<void*>(0), boost::bind(ldap_unbind_ext, handle, (LDAPControl**)(0), (LDAPControl**)(0))
  );
  if (result != LDAP_SUCCESS ) {
    throw LDAPException(
      std::string("ldap_simple_bind_s error: ").append(
        ldap_err2string(result)
      )
    );
  }
  struct timeval timeout;
  if (vm.count("timeout")) {
    timeout.tv_sec = vm["timeout"].as<int>();
    ldap_set_option(handle, LDAP_OPT_NETWORK_TIMEOUT, &timeout);
  }
  std::vector<std::string> topics;
  if (vm.count("attribute")) {
    topics = vm["attribute"].as<std::vector<std::string> >();
  }

  static char* scope_str[] = {"base", "one-level", "subtree"}; 
  std::cout << "# base <" << vm["basedn"].as<std::string>() << "> with scope " 
            << scope_str[vm["scope"].as<size_t>()] << std::endl;
  std::cout << "# filter: " << vm["filter"].as<std::string>() << std::endl;
  std::cout << "# requesting: ";
  if(topics.empty()) std::cout << "ALL";
  else {
    std::copy(
      topics.begin(), topics.end(), 
      std::ostream_iterator<std::string>(std::cout, " ")
    );
  }
  std::cout << "\n\n";

  size_t const n = topics.size();
  boost::shared_array<const char*> c_arr(
    new const char*[ n + 1 ]
  );

  std::transform(
    topics.begin(), topics.end(), 
    c_arr.get(),
    std::mem_fun_ref(&std::string::c_str)
  );
  c_arr[n]=0;
 
  LDAPMessage *ldresult = 0;
    int r = ldap_search_ext_s( 
    handle, 
    vm["basedn"].as<std::string>().c_str(), 
    vm["scope"].as<size_t>(), 
    vm["filter"].as<std::string>().c_str(), 
    const_cast<char**>(c_arr.get()), 
    false,
    NULL,
    NULL,
    &timeout, 
    LDAP_NO_LIMIT,
    &ldresult
  );
  boost::shared_ptr<void> ldresult_guard(
    ldresult, ldap_msgfree
  );


  if (r != LDAP_SUCCESS) {
    throw LDAPException(
      std::string("ldap_search_st error: ").append(
        ldap_err2string(result)
      )
    ); 
  }
    
  for (
    LDAPMessage* lde = ldap_first_entry(handle, ldresult);
    lde != 0; lde = ldap_next_entry(handle, lde)
  ) {
    boost::shared_ptr<char> dn_str(
      ldap_get_dn( handle, lde ),
      ber_memfree
    );

    BerElement *ber = 0;
   
    std::cout << "dn: " << dn_str << std::endl;
    for( 
      char* attr = ldap_first_attribute(handle, lde, &ber);
      attr; attr = ldap_next_attribute(handle, lde, ber) 
    ) {
      boost::shared_ptr<void> attr_guard(
        attr, ber_memfree
      );
      boost::shared_array<struct berval*> values(
        ldap_get_values_len(handle, lde, attr),
        ldap_value_free_len
      );
    
      if (!values) continue;
      for(size_t i=0; values[i] != 0; ++i) {
        std::cout << attr << ": " << values[i]->bv_val << "\n"; 
      }
    }
     boost::shared_ptr<void> ber_guard(
       static_cast<void*>(0),boost::bind(ber_free, ber, 0)
    );
    
  }
  return 0;
}

