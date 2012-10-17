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
// File: ii_attr_utils.cpp
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id: ii_attr_utils.cpp,v 1.8.36.1 2010/04/08 12:49:02 mcecchi Exp $

#include "configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/ldif2classad/LDAPQuery.h"
#include "glite/wms/common/ldif2classad/LDAPSynchConnection.h"
#include "ldif2classad/LDAPForwardIterator.h"

#include "glite/wmsutils/exception/Exception.h"
#include "ii_attr_utils.h"

#include <boost/thread/mutex.hpp>
#include <boost/scoped_ptr.hpp>

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace utilities {
namespace ii_attributes {

using namespace glite::wms::common::ldif2classad;

namespace {
	
boost::mutex                                     f_mutex;
boost::scoped_ptr<ii_attributes::container_type> f_multi_attributes;
bool                                             f_is_glueschema;

struct attributetype
{
  enum { all, single, multi };
  attributetype(const std::string& a) {
    size_t n_pos = a.find("NAME");
    assert( n_pos != string::npos );
    size_t n_start = n_pos + 6;
    size_t n_len = a.find("'",n_start) - n_start;
    name = a.substr(n_start, n_len);
    singlevalue = a.rfind("SINGLE-VALUE")!=string::npos;
  } 
  std::string name;
  bool singlevalue;
};
	
std::vector<std::string> getTypes()
{
  
  string basedn("cn=subschema");
  string filter("objectclass=*");
  int timeout = 30;
  
  const configuration::NSConfiguration* ns_conf = configuration::Configuration::instance() -> ns();
  
  boost::scoped_ptr<LDAPConnection> connection;
  connection.reset(new LDAPSynchConnection ( basedn, ns_conf -> ii_contact(), ns_conf -> ii_port(), timeout ));
  
  vector<string> attributes;
  attributes.push_back("attributeTypes");
  LDAPQuery query(connection.get(), filter, attributes, LDAPQuery::SCOPE_BASE );	
  vector<string> attributetypes;
  try {
    connection -> open();
    query.execute();
    LDAPForwardIterator ldap_it(query.tuples());
    
    ldap_it.first();
    (*ldap_it).EvaluateAttribute("attributeTypes",attributetypes);
  }
  catch( glite::wmsutils::exception::Exception& e ) {
    cout << e.what() << endl;
  }
  return attributetypes;
}

std::vector<std::string> 
filterTypes(std::vector<std::string>& attributetypes, int m, bool glue_schema)
{
  std::vector<std::string> value; 
  for(	std::vector<std::string>::const_iterator it = attributetypes.begin();
	it != attributetypes.end(); it++ ) {
    bool ok = false;
    switch( m ) {
    case attributetype::single: ok =  attributetype(*it).singlevalue;  break; 
    case attributetype::multi:  ok = !attributetype(*it).singlevalue;  break; 
    case attributetype::all:    ok = true;                             break;
    }	
    if( ok ) {
      
      std::string n(attributetype(*it).name);
      if( glue_schema ) {
	if( n.substr(0,4) == string("Glue") ) value.push_back( n );
      }
      else if( isupper(n[0]) ) value.push_back( n ); 
    }	
  }
  return value;	 
}

ii_attributes::container_type getMultiValued()
{
  return *f_multi_attributes;
}

void init()
{
    if( !f_multi_attributes.get() ) {
      boost::mutex::scoped_lock l( f_mutex );
      if( !f_multi_attributes.get() ) {
	
        vector<string> attributetypes( getTypes() );
    
        f_multi_attributes.reset( new vector<string>( filterTypes( attributetypes, attributetype::multi, true) ) );
        f_is_glueschema = !f_multi_attributes -> empty();
        if( !f_is_glueschema ) {
	  f_multi_attributes.reset( new vector<string>( filterTypes( attributetypes, attributetype::multi, false) ) );
        }
      }
    }  
}

} // hidden namespace closure

std::pair<ii_attributes::const_iterator, ii_attributes::const_iterator> 
multiValued()
{
  init();	
  return std::make_pair( f_multi_attributes -> begin(), f_multi_attributes -> end() );
} 

bool isGlueSchema()
{
  init();	
  return f_is_glueschema;
}

} // namespace ii_attributes
} // namespace utilities
} // namespace common
} // namespace wms 
} // namespace glite
