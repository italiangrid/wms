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
 *  filename  : LDAPForwardIterator.cpp
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001, 2002 by INFN
 ***************************************************************************/

// $Id: LDAPForwardIterator.cpp,v 1.3.36.1.4.1 2012/02/15 09:26:35 mcecchi Exp $


/**
 * @file LDAPForwardIterator.cpp
 * @brief Contains implementations for LDAP Forward Iterator.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */

/** This class header file. */
#include "LDAPForwardIterator.h"
/** The LDIF header file. */
#include "glite/wms/common/ldif2classad/LDIFObject.h"
/** The LDAP Connection header file. */
#include "glite/wms/common/ldif2classad/LDAPConnection.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {

/**
 * Constructor
 * @param q a reference to an LDAPQuery result object.
 */
LDAPForwardIterator::LDAPForwardIterator( generic_search_result_t* result ) 
{
  current_entry = NULL;
  search_result = result;
}

/** 
 * Returns the distinguished name of the ldap entry pointed to.
 */ 
string LDAPForwardIterator::ldap_dn()
{ 
  return current_entry != NULL ? current_entry -> distinguished_name() : string();
}

/**
 * Rewinds the LDAP forward iterator and returns the first entry.
 * After use of this method, the forward iterator is addressed 
 * to the first query result record.
 * @return a pointer to the first result entry, NULL is no entry is available.
 */
generic_result_entry_t* LDAPForwardIterator::first()
{
  if( current_entry != NULL ) {
    
    delete current_entry;
    current_entry = NULL;
  }
  
  if( search_result != NULL && 
      search_result -> good() ) {  
    
    current_entry = search_result -> make_first_entry();
  }
  
  return (current_entry);
}

/**
 * Moves the iterator to next result entry.
 * @return a pointer to the next result entry, or NULL if no further entry is available.
 */
generic_result_entry_t* LDAPForwardIterator::next()
{ 
  if( current_entry != NULL ) {  
    
    current_entry -> next();
    
    if( !current_entry -> good() ) {
      
      delete current_entry;
      current_entry = NULL;
    }
  } 
  return current_entry;
}
  
/**
 * Gets the current result entry.
 * @return a pointer to the current result entry the iterator points to.
 */
generic_result_entry_t* LDAPForwardIterator::current() const
{ 
  return current_entry;
}

LDIFObject* LDAPForwardIterator::operator->()
{
if( current_entry == NULL ) throw LDAPNoEntryEx();
return current_entry -> value();	
}

const LDIFObject& LDAPForwardIterator::operator*()
{ 
  if( current_entry == NULL ) throw LDAPNoEntryEx();

  return *(current_entry -> value());
}
/**
 * Destructor.
 */
LDAPForwardIterator::~LDAPForwardIterator()
{
  if( current_entry != NULL ) delete current_entry;
}

} // namespace ldif2classad
} // namespace common
} // namespace wms
} // namespace glite
