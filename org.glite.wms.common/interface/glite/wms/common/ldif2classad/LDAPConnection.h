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
 *  filename  : LDAPConnection.h
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001, 2002 by INFN
 ***************************************************************************/

/**
 * @file LDAPConnection.h
 * @brief This file contains definition for LDAP Connections.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */

// $Id: LDAPConnection.h,v 1.1.36.1 2010/04/08 12:49:01 mcecchi Exp $
// $Date: 2010/04/08 12:49:01 $

#include<string>

#ifndef _LDAPCONNECTION_
#define _LDAPCONNECTION_

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {

class LDAPQuery;
class LDIFObject;

/**
 * Interface to generic result entry.
 * This class models a basic standard interface to 
 * LDAPConnection search result entries.
 */
struct generic_result_entry_t
{
  /** 
   * Abstract distinguished name extractor.
   * @return the distinguished name of the result entry.
   */
  virtual std::string distinguished_name() const = 0;
  /** 
   * Abstarct validation test.
   * @return whether the entry is good, or not.
   */
  virtual bool good() const = 0;
  /** 
   * Abstarct next entry assignment.
   * @return whether the assignment has been succesfull, or not.
   */
  virtual bool next() = 0;
  virtual LDIFObject* value() = 0;
  /** 
   * Destructor.
   */
  virtual ~generic_result_entry_t() {}
};

/**
 * Interface to generic search result.
 * This class models a basic standard interface 
 * to LDAPConnection search results.
 */
struct generic_search_result_t
{
  /** 
   * Destructor.
   */
  virtual ~generic_search_result_t() {}
  
  /** 
   * Abstract validation test.
   * @return whether the entry is good, or not.
   */
  virtual bool good() const = 0;
  /** 
   * Abstract empty test.
   * @return whether there is a result, or not.
   */
  virtual bool empty() const = 0;
  
  /**
   * Abstract entry factory method.
   * @return the first result entry.
   */
  virtual generic_result_entry_t* make_first_entry() const = 0;
};

/**
 * Defines an LDAPConnection interface.
 * This class models a basic standard interface to an LDAPConnection
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */
struct LDAPConnection
{
  /**
   * Opens a connection to the Server. 
   * This function must be overridden by all classes extending LDAPConnection.
   * @return false.
   */
  virtual bool open() = 0;
  /**
   * Closes a connection to the Server. 
   * This function must be overridden by all classes extending LDAPConnection.
   * @return false.
   */  
  virtual bool close() = 0;
  /**
   * Searches a string in a message.    
   * This function must be overridden by all classes extending basic LDAPConnection.
   * @return false.
   */  
  virtual generic_search_result_t* execute( LDAPQuery* ) = 0;
  
  /**
   * Shows wheter tconnection is established or not.
   * @return true if connection is established, false otherwise.
   */
  virtual bool is_established() const = 0;
  /**
   * Get next ldap entries
   * This function must be overridden by all classes extending basic LDAPConnection 
   * in order to provide che concrete ldap entry extractor depending on connection 
   * type.
   * @return a pinter to next entry.
   */  

  /** 
   * Destructor.
   */
  virtual ~LDAPConnection() {}
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










