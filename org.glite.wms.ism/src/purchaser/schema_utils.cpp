/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// File: schema_utils.cpp
// Author: Salvatore Monforte
// $Id$
#include <sys/time.h>
#include <ldap.h>
#include <lber.h>

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <exception>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wmsutils/exception/Exception.h"

#include "schema_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"

namespace cfg = glite::wms::common::configuration;
namespace ut = glite::wms::common::utilities;

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

namespace {
	
boost::mutex f_mutex;
boost::shared_ptr<std::vector<std::string> > f_multi_attributes;
boost::shared_ptr<std::vector<std::string> > f_number_attributes;
boost::regex f_regex(
  "^\\(\\s*(?:\\S.*)\\s+NAME\\s+'([^']*)'(?:.*)\\s+EQUALITY\\s+(\\S+)(?:.*)\\)"
);

struct attributetype
{
  enum { single = 0x01, multi, all, number = 0x08 };

  attributetype(const std::string& a) {
  
    boost::smatch m;
    if (boost::regex_match(a, m, f_regex) ) {
      name = std::string(m[1].first, m[1].second);
      numbervalue = std::string(m[2].first, m[2].second) == "integerMatch";     
      singlevalue = a.rfind("SINGLE-VALUE")!= std::string::npos;
    }
  } 
  std::string name;
  bool singlevalue;
  bool numbervalue;
};
	
std::vector<std::string> 
getTypes()
{
  const cfg::NSConfiguration* ns_conf(
    cfg::Configuration::instance() -> ns()
  );
  boost::shared_ptr<LDAP> handle(
    ldap_init(
      ns_conf->ii_contact().c_str(), ns_conf->ii_port()
    ), 
    ldap_unbind
  );
  
  std::vector<std::string> results;

  int result = ldap_simple_bind_s(handle.get(),0,0);
  
  if (result != LDAP_SUCCESS ) {
    return results;
  }

  LDAPMessage *ldresult = 0;
  ut::scope_guard ldresult_guard(
    boost::bind(ldap_msgfree, ldresult)
  );

  char* attr_req[] = {"attributeTypes", 0};
  struct timeval timeout;
  timeout.tv_sec = 30;
  ldap_set_option(handle.get(), LDAP_OPT_NETWORK_TIMEOUT, &timeout);

  result = ldap_search_st( 
    handle.get(), 
    "cn=subschema", 
    LDAP_SCOPE_BASE, 
    "objectclass=*", 
    attr_req, 
    false,
    &timeout, 
    &ldresult
  );

  if (result != LDAP_SUCCESS ) {
    return results;
  }
    
  for (
    LDAPMessage* lde = ldap_first_entry(handle.get(), ldresult);
    lde != 0; lde = ldap_next_entry(handle.get(), lde)
  ) {
    boost::shared_ptr<char> dn_str(
      ldap_get_dn( handle.get(), lde ),
      ldap_memfree
    );

    BerElement *ber = 0;
    ut::scope_guard ber_guard(
      boost::bind(ber_free, ber, 1)
    );
    
    for( 
      char* attr = ldap_first_attribute(handle.get(), lde, &ber);
      attr; attr = ldap_next_attribute(handle.get(), lde, ber) 
    ) {
      ut::scope_guard attr_guard(
        boost::bind(ldap_memfree, attr)
      );
      boost::shared_array<char *> values(
        ldap_get_values(handle.get(), lde, attr),
        ldap_value_free
      );
    
      if (!values) continue;
      for(size_t i=0; values[i] != 0; ++i) {
        results.push_back(values[i]); 
      }
    }
  }
  return results;
} 

std::vector<std::string> 
filterTypes(std::vector<std::string>& attributetypes, int m)
{
  std::vector<std::string> value; 
  for(	std::vector<std::string>::const_iterator it = attributetypes.begin();
	it != attributetypes.end(); it++ ) {

    boost::scoped_ptr<attributetype> at(
      new attributetype(*it)
    );

    if ( !boost::algorithm::istarts_with(at->name, "Glue") ) continue;
    
    bool ok = false;
    
    switch( m & 0x03) {
    case attributetype::single: ok =  at->singlevalue;  break; 
    case attributetype::multi:  ok = !at->singlevalue;  break; 
    case attributetype::all:    ok = true;              break;
    }
    if (m & 0x8) ok = at->numbervalue;	

    if( ok ) {
      value.push_back( at->name ); 
    }	
  }
  return value;	 
}
} // hidden namespace closure

bdii_schema_info_type::bdii_schema_info_type()
{
  if( !f_multi_attributes.get() ) {
  
    boost::mutex::scoped_lock l( f_mutex );
    if( !f_multi_attributes.get() ) {
	
      std::vector<std::string> attributetypes( getTypes() );
    
      f_multi_attributes.reset( 
        new std::vector<std::string>( 
          filterTypes( attributetypes, attributetype::multi) 
      ));
    
      f_number_attributes.reset( 
        new std::vector<std::string>( 
          filterTypes( attributetypes, attributetype::all | attributetype::number)
        ));
      }
    }
}

bdii_schema_info_type::bdii_schema_info_type(const bdii_schema_info_type&)
{
}

std::pair<
  std::vector<std::string>::const_iterator,
  std::vector<std::string>::const_iterator
> bdii_schema_info_type::multi_valued()
{
  return std::make_pair( 
    f_multi_attributes -> begin(), 
    f_multi_attributes -> end() 
  );
}

std::pair<
  std::vector<std::string>::const_iterator,
  std::vector<std::string>::const_iterator
> bdii_schema_info_type::number_valued()
{
  return std::make_pair(
    f_number_attributes -> begin(),
    f_number_attributes -> end()
  );
}

bdii_schema_info_type& 
bdii_schema_info()
{
  static bdii_schema_info_type _;
  return _;
}
}}}}
