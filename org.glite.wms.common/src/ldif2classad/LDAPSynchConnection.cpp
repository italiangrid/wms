/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://project.eu-egee.org/index.php?id=115 for details on the
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


/***************************************************************************
 *  filename  : LDAPSynchConnection.cpp
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001, 2002 by INFN
 ***************************************************************************/

/**
 * @file LDAPSynchConnection.cpp
 * @brief This files contains LDAP Synchronous connections implementation.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */
// $Id$

/** Includes header definitions. */
#include "glite/wms/common/ldif2classad/LDAPQuery.h"
#include "glite/wms/common/ldif2classad/LDAPSynchConnection.h"

#ifdef WITH_LDAP_EXCEPTIONS
#include "glite/wms/common/ldif2classad/exceptions.h"
#include "glite/wms/common/utilities/edgstrstream.h"
#endif 

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {

/**
 * Constructor.
 * @param b the base DN of the entry at which to start operations.
 * @param hn the host name of the LDAP server.
 * @param p the port of the LDAP server.
 * @param t the timeout used during binding to an LDAP server and searching its content.
 */
LDAPSynchConnection::LDAPSynchConnection(const std::string& base_dn, const std::string& source_name, int source_port, long tv_sec)
{
  this -> handle     = NULL;
  this -> base_dn     = base_dn;
  this -> source_name = source_name;
  this -> source_port = source_port;
  timeout.tv_sec = tv_sec;
  timeout.tv_usec = 0;
}

/**
 * Destructor.
 */
LDAPSynchConnection::~LDAPSynchConnection()
{
  close();
}


/**
 * Opens a connection to the Server. 
 * This function overrides virtual LDAP Connection one.
 * @return true on success, false otherwise.
 */
bool LDAPSynchConnection::open()
{
  bool result = false;
  
  close();

  LDAP* h_ldap = NULL;
  if ( (h_ldap = ldap_init( const_cast<char*>(source_name.c_str()), source_port) ) ) {
    
    ldap_set_option(h_ldap, LDAP_OPT_NETWORK_TIMEOUT, &timeout);
    
    if( (ldap_last_error=ldap_simple_bind_s(h_ldap,0,0)) == LDAP_SUCCESS )  {
      
      handle = h_ldap;
      result  = true;
    } 
#ifdef WITH_LDAP_EXCEPTIONS
    else {
      utilities::oedgstrstream source;
      source << "contact=" << source_name << ":" << source_port << ", dn=" << base_dn;      
      std::string connection_info(source.str());       
      std::string error_msg(ldap_err2string(ldap_last_error));
      throw ConnectionException(connection_info, std::string("ldap_simple_bind_s()"), error_msg); 
    }
#endif
  }
  else {
    if( h_ldap ) {
      ldap_unbind( h_ldap );
    } 
  }
  return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Search
//
generic_search_result_t* LDAPSynchConnection::execute( LDAPQuery* query ) 
{
  generic_search_result_t *search_result = NULL;

  if( is_established() ) {
    
    int    scope  = query -> scope();
    char*  filter = (char*)query -> filter().c_str();
    char** topics = make_topics( query -> topics() );
    
    LDAPMessage *ldresult = NULL;
    ldap_last_error = ldap_search_st( handle,
	(char*)base_dn.c_str(), 
	scope,
	filter,
	topics,
	false,
	&timeout, 
	&ldresult );
    
    delete[] topics;
    if( ldap_last_error != LDAP_SUCCESS ) {

#ifdef WITH_LDAP_EXCEPTIONS
	std::string str(ldap_err2string(ldap_last_error));
	utilities::oedgstrstream source;
	source << "contact=" << source_name << ":" << source_port << ", dn=" << base_dn
	       << "filter='" << filter << "'";
	std::string source_info(source.str());
	throw QueryException( source_info, "ldap_search_st", str);
#endif
    }
    else  {
	search_result = new ldap_search_result_t( ldresult, handle );
    }
  }
  return search_result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Close
//
bool LDAPSynchConnection::close()
{
  bool return_status = ( is_established() && 
			 ldap_unbind( handle ) == LDAP_SUCCESS);
   
  handle = NULL;
  
  return return_status; 
}


/**
 * Topics creator.
 * Creates a vector of pointer to topics to us AP Connection 
 * for record satisfing a filter, having a specified scope.
 */
char** LDAPSynchConnection::make_topics(const std::vector<std::string>& topic)
{
	std::vector<std::string>::const_iterator it;
  
  int i = 0;
  char** t = new char*[ topic.size()+1 ];
  
  for( it = topic.begin(); it != topic.end(); it++) {
    
    t[i++] = const_cast<char*>( (*it).c_str() );
  }
  t[i]=NULL;
  return (t);
}

/**
 * Shows wheter tconnection is established or not.
 * @return true if connection is established, false otherwise.
 */
bool LDAPSynchConnection::is_established() const 
{  
  return handle != NULL;
}

} // namespace ldif2classad 
} // namespace common
} // namespace wms
} // namespace glite 
