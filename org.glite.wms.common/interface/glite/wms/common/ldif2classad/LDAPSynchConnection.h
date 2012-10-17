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
 *  filename  : LDAPSynchConnection.h
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001, 2002 by INFN
 ***************************************************************************/

// $Id: LDAPSynchConnection.h,v 1.1.36.1 2010/04/08 12:49:01 mcecchi Exp $

/**
 * @file LDAPSynchConnection.h
 * @brief This files contains definitions for LDAP Synchronous connections.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */

#ifndef _LDAPSYNCHCONNECTION_
#define _LDAPSYNCHCONNECTION_ 

#include <sys/time.h>
#include <ldap.h>
#include <lber.h>

#ifdef WITH_LDAP_EXCEPTIONS
#include "exceptions.h"
#endif 

/** Includes the superclass's definitions. */
#include "LDAPConnection.h"
#include "LDIFObject.h"

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {

/**
 * Concrete LDAP result entry.
 * This class specialises the standard interface 
 * for concrete LDAPConnectionSynchConnection search result entries.
 */
class ldap_result_entry_t : public generic_result_entry_t
{
public:
  /**
   * Costructor
   * @param ldentry the result  as  returned  by  a  call  to ldap_result or ldap_search_s or ldap_search_st
   * @param ld the andle to ldap connection.
   */
  ldap_result_entry_t(LDAPMessage* ldentry, LDAP* ld)
  { 
    this -> ldentry = ldentry;
    this -> ld      = ld; 
  }  
  
  /**
   * Concrete next entry assignment.
   * @return whether the next entry has been succesfully addressed, or not.
   */
  bool next()
  {
    if( this -> good() ) {
      LDAPMessage* lne = ldap_next_entry( ld, ldentry );
      return ( (ldentry = lne) != NULL );
    }
    return false;
  }
  /**
   * Concrete validation test
   * @return whether the entry is valid or not.
   */
  bool good () const 
  {
    return (ld != NULL && ldentry != NULL);
  }
  /**
   * Concrete distinguished name extractor.
   * @return a string representation of entry's distinguished name
   */
  std::string distinguished_name() const 
  {
    char *dn_ptr = ldap_get_dn( ld, ldentry );
    std::string dn(dn_ptr);
    ber_memfree(dn_ptr);
    return dn;
  }
  /**
   * Concrete LDIFObject extractor.
   * @return a poiter to a LDIFObject.
   */
  LDIFObject* value() 
  {
    BerElement *ber = NULL;
    
    object = LDIFObject();
    char *attribute = NULL;
    for(attribute = ldap_first_attribute(ld, ldentry, &ber); 
	attribute; attribute = ldap_next_attribute(ld, ldentry, ber) ) {
      
      char **values;
      values = ldap_get_values(ld, ldentry, attribute);
      if( values ) {
	for(int i=0; values[i]!=NULL; i++)
	  
	  object.add( (std::string)attribute, (std::string)values[i] );
	
	ber_memfree(attribute);
	ldap_value_free(values);
      }
      
#ifdef WITH_LDAP_EXCEPTIONS
      else {
	
	if( ber != NULL ) ber_free( ber, 0 );
	std::string error_msg( ldap_err2string( ldap_result2error(ld, ldentry, 0) ) );
	throw UndefinedValueException(std::string("LDIFObject::value()"), std::string("ldap_get_values()"), error_msg);
      }
#endif
    }
    if( ber != NULL ) ber_free( ber, 0 );
    
    return &object;
  } 
private:
  /* A pointer to the concrete LDAP result entry */
  LDAPMessage *ldentry;
  /* A pointer to the concrete LDAP connection */
  LDAP        *ld;    
  /* A pointer to the object representation of the entry @see LDIFObject.h */
  LDIFObject  object;
};

/**
 * Concrete LDAP result entry.
 * This class specialises the standard interface 
 * for concrete LDAPConnectionSynchConnection search result entries.
 */
class ldap_search_result_t : public generic_search_result_t 
{
public:
  /**
   * Costructor
   */
  ldap_search_result_t(LDAPMessage* ldresult, LDAP* ld) 
  {
    this -> ldresult = ldresult;
    this -> ld       = ld;
  }
 
  ~ldap_search_result_t() 
  {
    if( ldresult != NULL )
      ldap_msgfree(ldresult);
  }
  /**
   * Concrete Validation test
   */
  bool good() const { return ( ldresult != NULL && ld != NULL ); }
  bool empty() const { return ( good() && (ldap_count_entries(ld, ldresult) == 0) ); }
				/**
   * Concrete Entry Factory Method.
   * @return the first ldap result entry. The memory object should be destroid by explicitally calling delete.
   */
  generic_result_entry_t* make_first_entry() const
  {
    ldap_result_entry_t* r = NULL;
    
    if( good() ) {
      LDAPMessage* ldap_1st_entry = ldap_first_entry( ld, ldresult );
#ifdef WITH_LDAP_EXCEPTIONS      
      if( ldap_1st_entry == NULL ) {
	      std::string error_msg( ldap_err2string( ldap_result2error(ld, ldresult, 0) ) );
	throw QueryException(std::string("make_first_entry"), std::string("ldap_first_entry()"), error_msg);
      }
#endif
      r = new ldap_result_entry_t( ldap_1st_entry, ld );
    }
    return (r);
  }
  
private:
  /* A pointer to the concrete LDAP result returned by ldap search */
  LDAPMessage *ldresult;
  /* A pointer to the concrete LDAP connection */
  LDAP        *ld;    
};

/**
 * A Concrete Synchronous connection to an LDAP Server.
 * This class implements all virtual superclass methods in order
 * to provide a synchronous concrete connection to the LDAP Server.
 * @see LDAPConnection
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */
class LDAPSynchConnection : public LDAPConnection
{
public:
  /**
   * Constructor.
   */
  LDAPSynchConnection(const std::string&, const std::string&, int, long = 15);
  
  /**
   * Distructor.
   */
  virtual ~LDAPSynchConnection();
  
  /**
   * Opens a connection to the Server. 
   * This function overrides virtual LDAP Connection one.
   * @return true on success, false otherwise.
   */
  bool open();
  /**
   * Closes a connection to the Server. 
   * This function overrides virtual LDAP Connection one.
   * @return true on success, false otherwise.
   */
  bool close();
  /**
   * Performs LDAP search  operations using ldap_search_st() 
   * allowing a timeout to be specified.
   * @param q the query which will be executed.
   * @return a pointer to generic search result. This pointer should be explicitly freed.
   */
  generic_search_result_t* execute(LDAPQuery* q); 
 /**
   * Conenction test wheter established or not.
   * @return true if connection is established, false otherwise.
   */
  bool is_established() const;
  std::string error() const { return std::string( ldap_err2string(ldap_last_error) ); }

private:
  char** make_topics( const std::vector<std::string>& );

private:
  /** A connection timeout. */
  struct timeval timeout;
  int source_port;
  std::string source_name;
  std::string base_dn;
  LDAP* handle;
  int ldap_last_error;
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
