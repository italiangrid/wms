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
/***************************************************************************
 *  filename  : LDAPQuery.cpp
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001, 2002  by INFN
 ***************************************************************************/

// $Id$

/**
 * @file LDAPQuery.cpp
 * @brief This files implemnts an LDAP Query Object.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */

/** This class header file. */
#include "glite/wms/common/ldif2classad/LDAPQuery.h"
/** The LDAP Connection header file. */
#include "glite/wms/common/ldif2classad/LDAPConnection.h"

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {


/**
 * Constructor.
 * This object queries a database through an LDAP Connection 
 * for record satisfing a filter, having a specified scope.
 */ 
LDAPQuery::LDAPQuery(LDAPConnection* connection, const std::string& filter,const std::vector<std::string>& topics, int scope) 
{
  this -> connection    = connection;
  
  search_filter = filter;
  search_scope  = scope;
  search_topic  = topics;
  search_result = NULL;
}

/**
 * Destructor.
 */
LDAPQuery::~LDAPQuery()
{
  if( search_result != NULL ) delete search_result;
}
/**
 * Execute the query.
 * @return whether the query has been correctly ececuted, or not.
 */
bool LDAPQuery::execute()
{
  if( search_result != NULL ) {
    
    delete search_result;
    search_result = NULL;
  }  
  return (search_result = connection -> execute(this)) != NULL;
}
/**
 * Result Tuples.
 * @return the resulting tuples, NULL if no result is available.
 */
generic_search_result_t* LDAPQuery::tuples() const
{ 
  return search_result; 
}

std::string  LDAPQuery::what() const
{
  return std::string("filter = ") + search_filter;
}

} // namespace ldif2classad
} // namespace common
} // namespace wms
} // namespace glite

