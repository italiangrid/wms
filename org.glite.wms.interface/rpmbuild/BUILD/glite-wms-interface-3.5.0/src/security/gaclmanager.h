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

#ifndef GLITE_WMS_WMPROXY_WMPGACLMANAGER_H
#define GLITE_WMS_WMPROXY_WMPGACLMANAGER_H

extern "C" {
#include "gridsite.h"
}

#include <string>
#include<vector>

namespace glite {
namespace wms {
namespace wmproxy {
namespace security {
/**
 * WMPGaclManager class provides a set of utility methods for handling
 * gacl files for Grid Users authorization.
 *
 * @brief utility methods for user gacl mapping
 * @version 1.0
 * @date 2005
 * @author Marco Sottilaro <marco.sottilaro@datamat.it>
*/


typedef GRSTgaclPerm WMPgaclPerm ;


class GaclManager
{

public:
   /**
      Credential types
   */
   enum WMPgaclCredType {
      WMPGACL_UNDEFCRED_TYPE,
      WMPGACL_ANYUSER_TYPE,
      WMPGACL_PERSON_TYPE,
      WMPGACL_DNLIST_TYPE,
      WMPGACL_VOMS_TYPE,
      WMPGACL_DNS_TYPE
   } ;

   /**
   *  Constructor
   *  @param file path location of the gacl file
   *  @param create creates a new file if it is true
   *  @throw GaclException if  the "create" input paramater is set to false and the gacl file doesn't exist
   */
   GaclManager (const std::string& file, bool create=false);
   /**
   * destructor
   */
   ~GaclManager( ) ;
   /**
   * Checks if the gacl has the entry with the specified credential type and rawvalue
   * @param type the type of the credential of the entry to be looked for
   * @param rawvalue the raw that the credential has to contain
   * @return true if the gacl contains the specified credential entry, false otherwise
   */
   bool hasEntry(const WMPgaclCredType& type, const std::string rawvalue="");

   /**
   *  Adds a credential entry to the gacl
   *  @param type credential type
   *  @param rawvalue credential raw (user-dn, fqan, etc...)
   *  @param permssion allowed permission (the default value is read permission )
   *  @throw GaclException if the credential already exists in the gacl
   */
   void addEntry ( const WMPgaclCredType& type,
                   const std::string& rawvalue,
                   const WMPgaclPerm& permission=WMPGACL_READ) ;
   /**
   *  Adds a set of credential entries to the gacl
   *  @param vect vector of credential (credential type , rawvalue )
   *  @throw GaclException if any credential in the vector already exists in the gacl
   */
   void addEntries ( const std::vector<std::pair<WMPgaclCredType, std::string> > &vect ) ;
   /**
   *  Removes from the gacl file the entry of the specified credential
   *  @param file path location of the gacl file
   *  @param type credential type
   *  @param rawvalue credential raw (user-dn, fqan, etc...)
   *       @param errors description of errors occurred during the removal
   *       @return 0 if the removal operation is successfully performed, -1 in case of any errors occured
   */
   int removeEntry (const WMPgaclCredType& type,
                    const std::string& rawvalue,
                    std::string& errors) ;
   /**
   *  Removes from the gacl file a set of entry
   *  @param vect vector of credential (credential type , rawvalue )
   *  @throw GaclException if any credential in the vector doesn't exist in the gacl
   */
   void removeEntries (const std::vector<std::pair<WMPgaclCredType, std::string> > &vect ) ;

   /**
   *  Checks whether user has permission to perform one or more actions (like read, write, etc...)
   *  @param type credential type
   *  @param rawvalue credential raw (user-dn, fqan, etc...)
   *  @param permission permission to be checked (it can be also express more types of action : read | write | ....  ; | = or )
   *  @return true if permission is allowed
   *  @throw GaclException if the credential doesn't exist in the gacl and
   *     in case of any format error in the gacl
   */
   bool checkAllowPermission (const WMPgaclCredType& type,
                              const std::string& rawvalue,
                              const WMPgaclPerm& permission) ;

   /**
   *  Checks whether user doesn't have permission to perform one or more actions (like read, write, etc...)
   *  @param type credential type
   *  @param rawvalue credential raw (user-dn, fqan, etc...)
   *  @param permission permission to be checked (it can be also express more types of action : read | write | ....  ; | = or )
   *  @return true if permission is denied
   */
   bool checkDenyPermission (const WMPgaclCredType& type,
                             const std::string& rawvalue,
                             const WMPgaclPerm& permission);
   /**
   *  Allows permission
   *  @param type credential type
   *  @param rawvalue credential raw (user-dn, fqan, etc...)
   *  @param permission permission to be set (it can be also express more types of action : read | write | ....  ; | = or )
   *  @param unset unsetting the old permission if it sets to  true (false is the default value)
   *  @throw GaclException if the credential doesn't exist in the gacl
   */
   void allowPermission( const WMPgaclCredType& type,
                         const std::string& rawvalue,
                         const WMPgaclPerm& permission,
                         const bool& unset_perm = false  );
   /**
   *  Denies permission
   *  @param type credential type
   *  @param rawvalue credential raw (user-dn, fqan, etc...)
   *  @param permission permission to be denied (it can be also express more types of action : read | write | ....  ; | = or )
   *  @param unset unsetting the old permission if it sets to  true (false is the default value)
   *  @throw GaclException if the credential doesn't exist in the gacl
   */
   void denyPermission(const WMPgaclCredType& type,
                       const std::string& rawvalue,
                       const WMPgaclPerm& permission,
                       const bool& unset_perm = false  );
   /*
   *  Sets allowed permission (unsetting the old permission)
   *  @param type credential type
   *  @param rawvalue credential raw (user-dn, fqan, etc...)
   *  @param permission permission to be set (it can be also express more types of action : read | write | ....  ; | = or )
   *  @throw GaclException if the credential doesn't exist in the gacl
   */
   void setAllowPermission( const WMPgaclCredType& type,
                            const std::string& rawvalue,
                            const WMPgaclPerm& permission );
   /*
   *  Sets denied permission (resetting the old permission)
   *  @param type credential type
   *  @param rawvalue credential raw (user-dn, fqan, etc...)
   *  @param permission permission to be denied (it can be also express more types of action : read | write | ....  ; | = or )
   *  @throw GaclException if the credential doesn't exist in the gacl
   */
   void setDenyPermission(const WMPgaclCredType& type,
                          const std::string& rawvalue,
                          const WMPgaclPerm& permission );

   /**
   *  Gets the Acl item strings associated to the specified credential type
   *  (expect for any-user credential type)
   *  @param type credential type
   *  @return a vector with the item strings
   *  @throw GaclException in case of any error
   */
   const std::vector<std::string> getItems(const WMPgaclCredType& type);
   /**
   * Returns the string describing the type of credential specified in the input parameter
   * @return the string with the description, empty string if the input credential is not valid
   */
   std::string getCredentialTypeString ( const WMPgaclCredType& type) ;
   /**
   * Checks which credential entry types are present in the gacl file
   * @returns true if the credential type is present in the gacl file
   */
   bool checkCredentialEntries(const std::string& type) ;
   /**
   *  Saves the gacl to the file
   *  @param file location where to save the file
   *  @return WMPGACL_SUCCESS if the file has been successfully saved
   *        WMPGACL_ERROR in case of any error
   */
   int saveGacl ( const std::string& file ) ;
   /**
   *  Saves the gacl in a file
   *  @return WMPGACL_SUCCESS if the file has been successfully saved
   *        WMPGACL_ERROR in case of any error
   */
   int saveGacl (  ) ;

   /**
   * Returns the pathname of the gacl file
   * @return the string containing the pathname
   */
   std::string getFilepath ( );

   /**
   *  Checks if the gacl file exists
   *  @return true if the file exists
   */
   bool gaclExists ();
   /**
   *       Static method that checks if the gacl file exists
   *       @param the gacl filepath
   *       @return true if the file exists
   */
   static bool gaclExists (const std::string& file);

   /**
   * Free the memory used by an existing ACL, and the memory used by any entries and credentials it may contain.
   */
   void gaclFreeMemory( );

   /**
   *  Type of permission: NO PERMISSION (init value)
   */
   static const unsigned int WMPGACL_NOPERM ;

   /**
   *  Type of permission: read
   */
   static const unsigned  int WMPGACL_READ    ;

   /**
   *  Type of permission: exec
   */
   static const unsigned int WMPGACL_EXEC;

   /**
   *  Type of permission: list
   */
   static const unsigned int WMPGACL_LIST;

   /**
   *  Type of permission: write
   */
   static const unsigned int WMPGACL_WRITE ;

   /**
   *  Type of permission: admin
   */
   static const unsigned int WMPGACL_ADMIN ;
   // ====================================
   /**
   * Credential type: undef
   */
   static const char* WMPGACL_NONE_CRED;
   /**
   * Credential type: any-user
   */
   static const char* WMPGACL_ANYUSER_CRED;
   /**
   * Credential type: person
   */
   static const char* WMPGACL_PERSON_CRED ;
   /**
   * Credential type: dnlist
   */
   static const char* WMPGACL_DNLIST_CRED ;
   /**
   * Credential type: voms
   */
   static const char* WMPGACL_VOMS_CRED ;
   /**
   * Credential type: dns
   */
   static const char* WMPGACL_DNS_CRED ;

   // ====================================

   /**
   * XML credential tag for   any-user
   */
   static const char* WMPGACL_ANYUSER_TAG;
   /**
   *XML  credential tag for person credential
   */
   static const char* WMPGACL_PERSON_TAG ;
   /**
   * XML credential tag for dnlist credential
   */
   static const char* WMPGACL_DNLIST_TAG ;
   /**
   * XML credential tag for DNS credential
   */
   static const char* WMPGACL_DNS_TAG ;
   /**
   * XML credential tag for VOMS credential
   */
   static const char* WMPGACL_VOMS_TAG ;

   // ====================================

   /**
   *  Gacl default filenames
   */
   static const char* WMPGACL_DEFAULT_FILE ;
   static const char* WMPGACL_DEFAULT_DRAIN_FILE ;
   /**
   * Function result : SUCCES
   */
   static const int WMPGACL_SUCCESS;
   /**
   * Function result: ERROR
   */
   static const int WMPGACL_ERROR ;

private :
   /**
   *  Loads the gacl from a file
   *  @param file path location of the gacl file
   */
   void loadFromFile (const std::string& file) ;
   /**
   *  Creates a new gacl
   */
   void newGacl () ;
   /**
   *  Sets the type of credential
   */
   void setCredentialType(const WMPgaclCredType& type, const std::string& rawvalue);
   /**
   *  Loads credential from the gacl file
   *  @param type credential type
   *  @param rawvalue credential raw (user-dn, fqan, etc...)
   *  @throw GaclException if the credential doesn't exist in the gacl
   */
   int loadCredential ( const WMPgaclCredType& type,
                        const std::string& rawvalue) ;
   /**
    *  looks for credential
   */
   int loadCredential () ;

   /**
    *  creates new credential
   */
   void newCredential ( ) ;

   // attributes
   /**
   *  pointer to the gacl struct
   */
   GRSTgaclAcl *gaclAcl  ;
   /**
   *  pointer to the credential entry struct
   */
   GRSTgaclEntry *gaclEntry;
   /**
   *  pointer to the credential struct
   */
   GRSTgaclCred *gaclCred  ;
   /**
   *  pointer to the user struct
   */
   GRSTgaclUser *gaclUser ;
   /**
   * allowed and denied flags
   */
   WMPgaclPerm  gaclAllowed ;
   WMPgaclPerm  gaclDenied ;

   /**
   * path location to the gacl file
   */
   std::string gaclFile;

   /**
   * credential types = WMPGACL_XXXX_CRED
   */
   std::string credType;
   /**
   * pair <rawname, rawvalue>
   *  in the gacl : <rawname><rawvalue></rawname>
   */
   std::pair<std::string, std::string> rawCred;
};

}}}}

#endif // GLITE_WMS_WMPROXY_WMPGACLMANAGER_H
