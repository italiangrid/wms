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
 *  filename  : LDAPForwardIterator.h
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001, 2002 by INFN
 ***************************************************************************/

/**
 * @file LDAPForwardIterator.h
 * @brief Contains definitions for LDAP Forward Iterator.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */

// $Id: LDAPForwardIterator.h,v 1.2.36.1 2010/04/08 12:49:02 mcecchi Exp $
// $Date: 2010/04/08 12:49:02 $

#ifndef _LDAP_FORWARD_ITERATOR_
#define _LDAP_FORWARD_ITERATOR_

#include <string>

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {

class LDAPNoEntryEx
{
};

/** A class representing LDIF Object. */
class LDIFObject;

/** Abstract interfaces to search results and entries */
struct generic_search_result_t;
struct generic_result_entry_t;

/**
 * A forward iterator for LDAP Objects.
 * It applies to LDAP Query results a powerful means of reading.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */
class LDAPForwardIterator
{
 public:
  /**
   * Constructor
   * @param results pointer to LDAP result to iterate.
   */
  LDAPForwardIterator(generic_search_result_t *results);
  /**
   * Rewinds the LDAP forward iterator and returns the first entry.
   * After use of this method, the forward iterator is addressed 
   * to the first query result record.
   * @return a pointer to the first result entry, NULL is no entry is available.
   */
  generic_result_entry_t* first();
  /**
   * Moves the iterator to next result entry.
   * @return a pointer to the next result entry, or NULL if no further entry is available.
   */
  generic_result_entry_t* next();
  /**
   * Gets the current result entry.
   * @return a pointer to the current result entry the iterator points to.
   */
  generic_result_entry_t* current() const;
  
  /** 
   * DN extractor   
   * @return the distinguished name of the result entry the iterator points to.
   */ 
  std::string ldap_dn();
  
  /**
   * LDIFObject extractor.
   * @return a reference to the LDIFObject the iterator points to.
   */
  const LDIFObject& operator*();
  LDIFObject* operator->(); 
  /**
   * Destructor.
   */
  ~LDAPForwardIterator();
  
private: 
  /** result entry the iterator points to */
  generic_result_entry_t *current_entry;
  generic_search_result_t* search_result; 
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
