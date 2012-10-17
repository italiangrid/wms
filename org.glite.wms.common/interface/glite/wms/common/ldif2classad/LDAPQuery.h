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
 *  filename  : LDAPQuery.h
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001, 2002 by INFN
 ***************************************************************************/

// $Id:

/**
 * @file LDAPQuery.cpp
 * @brief This files contains definition for an LDAP Query Object.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */

#ifndef _LDAP_QUERY_
#define _LDAP_QUERY_

#include <string>
#include <vector>

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {

/** An LDAP Connection. */
class LDAPConnection;

/** generic search result forward declaration */
struct generic_search_result_t;

/**
 * Queries a Database.
 * This class uses LDAP Connection base class to query a database for info.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */
class LDAPQuery
{
public:
  /* search scopes */
  enum search_scope_t { SCOPE_BASE, SCOPE_ONELEVEL, SCOPE_SUBTREE };

  /**
   * Constructor.
   * This object queries a database through an LDAP Connection 
   * for record satisfing a filter, having a specified scope.
   */ 
  LDAPQuery(LDAPConnection*, const std::string&, const std::vector<std::string>&, int s = SCOPE_SUBTREE);
  
  /**
   * Destructor.
   */
  ~LDAPQuery();
  
  /**
   * Returns scope attribute value. 
   * @return the scope.
   */
  
  int scope() const { return search_scope; }
  /**
   * Returns the applied filter.
   * @return the filter.
   */
  
  std::string filter() const { return search_filter; }
  /**
   * Returns the topic attribute.
   * @return the topic.
   */
  
  const std::vector<std::string>& topics() const { return search_topic; }
  /**
   * Result Tuples.
   * @return the resulting tuples, NULL if no result is available.
   */
  
  generic_search_result_t* tuples() const;
  /**
   * Execute the query.
   * @return whether the query has been correctly ececuted, or not.
   */
  bool execute();
 
  std::string what() const;
  
private:

  LDAPConnection          *connection;    /**< The LDAP Connection to data. */
  generic_search_result_t *search_result; /* < Pointer to generic search result */
  std::string              search_filter; /**< A string representation of the filter to apply in the search. */
  std::vector<std::string> search_topic;  /**< A vector of string representing the attributes to retrieve. */
  int                      search_scope;  /**< The scope of the search  and  should  be  one  of the following:
                                               - LDAP_SCOPE_BASE,  to  search the object itself;
                                               - LDAP_SCOPE_ONELEVEL, to search the  object's  immediate  children;
                                               - LDAP_SCOPE_SUBTREE,  to search the object and all its descendents. 
					  */
};

} // namespace ldif2classad
} // namespace common
} // namespace wms
} // namespace glite


#endif


/*
  Local Variables:
  mode: c++
  End:
*/







