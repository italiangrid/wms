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

#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>

//Logger
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "utilities/logging.h"

// Exceptions
#include "utilities/wmpexceptions.h"
#include "utilities/wmpexception_codes.h"

//Utilities
#include "utilities/utils.h"

#include "gaclmanager.h"
#include "authorizer.h"

#include <iostream>
#include <sstream>
#include <sys/stat.h>

namespace glite {
namespace wms {
namespace wmproxy {
namespace security {

using namespace std;

using namespace glite::wms::wmproxy::utilities;
namespace logger = glite::wms::common::logger;

// permission codes
const unsigned int GaclManager::WMPGACL_NOPERM = GRST_PERM_NONE;
const unsigned int GaclManager::WMPGACL_READ  = GRST_PERM_READ;
const unsigned int GaclManager::WMPGACL_EXEC = GRST_PERM_EXEC;
const unsigned int GaclManager::WMPGACL_LIST= GRST_PERM_LIST;
const unsigned int GaclManager::WMPGACL_WRITE = GRST_PERM_WRITE;
const unsigned int GaclManager::WMPGACL_ADMIN = GRST_PERM_ADMIN;

// user credentials
const char* GaclManager::WMPGACL_NONE_CRED  = "none";
const char* GaclManager::WMPGACL_ANYUSER_CRED = "any-user";
const char* GaclManager::WMPGACL_PERSON_CRED = "person" ;
const char* GaclManager::WMPGACL_DNLIST_CRED = "dn-list";
const char* GaclManager::WMPGACL_VOMS_CRED = "voms";
const char* GaclManager::WMPGACL_DNS_CRED = "dns";

// constants: credential tags
const char* GaclManager::WMPGACL_ANYUSER_TAG = "" ;
const char* GaclManager::WMPGACL_PERSON_TAG = "dn" ;
const char* GaclManager::WMPGACL_DNLIST_TAG = "url" ;
const char* GaclManager::WMPGACL_DNS_TAG = "hostname";
const char* GaclManager::WMPGACL_VOMS_TAG = "fqan";

// default filenames
const char* GaclManager::WMPGACL_DEFAULT_FILE = GRST_ACL_FILE;

const int GaclManager::WMPGACL_SUCCESS = 0;
const int GaclManager::WMPGACL_ERROR = -1;

namespace {

bool
compareFQAN(std::string const& gacl_fqan, std::string const& user_fqan)
{
   std::string gacl_regex(
      gacl_fqan.substr(
         std::string(std::string(GaclManager::WMPGACL_VOMS_TAG) + ':').size()
      )
   );
   // gacl_fqan is sort of URL encoded
   boost::replace_all(gacl_regex, ".", "[.]"); // . is reserved
   boost::replace_all(gacl_regex, "%2A", ".*"); // wildcard *
   boost::replace_all(gacl_regex, "+", " "); // space, not sure why it's not a %20
   boost::regex wildcard_match('^' + gacl_regex + '$', boost::regex::basic);
   return regex_match(user_fqan, wildcard_match);
}

}

GaclManager::GaclManager(const string& file, bool create)
   : gaclFile(file)
{
   credType = "";
   gaclAcl = 0;
   gaclCred = 0;
   gaclUser = 0;
   gaclEntry = 0;
   gaclAllowed = WMPGACL_NOPERM;
   gaclDenied = WMPGACL_NOPERM;
   if ( ! create ) {
      // checks if file exists
      if (gaclExists()) {
         // loads the existing file
         GRSTgaclInit();
         GaclManager::loadFromFile(gaclFile);
      } else {
         // exception: no such file
         std::ostringstream oss;
         oss <<  "gacl file not found (" << file << ")\n";
         throw GaclException (__FILE__, __LINE__,  "GaclManager::GaclManager",
                              WMS_GACL_ERROR, oss.str());
      }
   } else {
      // creates a new file
      newGacl();
   }
};

GaclManager::~GaclManager()
{
   if (gaclCred) {
      delete gaclCred;
   }
   if (gaclUser) {
      delete gaclUser;
   }
   if (gaclEntry) {
      delete gaclEntry;
   }
   if (gaclAcl) {
      delete gaclAcl;
   }
};

/**
* Free the memory used by an existing ACL, and the memory used by any entries and credentials it may contain.
*/
void GaclManager::gaclFreeMemory()
{
   edglog_fn("GaclManager::gaclFreeMemory");
   edglog(debug) << "cleaning memory by gaclFreeMemory"<< endl;
   if (gaclAcl) {
      GRSTgaclAclFree(gaclAcl );
   } else {
      edglog(debug) << "gacl is null" << endl;
   }
};

/*
*  adds a credential entry to the gacl
*  @param type credential type
*  @param rawvalue credential raw (user-dn, fqan, etc...)
*/
void GaclManager::addEntry(const WMPgaclCredType& type, const string& rawvalue,const WMPgaclPerm& permission)
{
   // sets the credential attributes
   setCredentialType(type,rawvalue);
   // checks if the credential already exist into the file
   if (loadCredential() == WMPGACL_SUCCESS) {
      std::ostringstream oss;
      oss <<  "unable to add the new credential entry to the gacl" ;
      oss << " (" << gaclFile << ")\n";
      oss << "reason: the entry already exists\ncredential type : " << getCredentialTypeString(type)  << "\n";
      oss <<"Input " << rawCred.first << ": " << rawvalue << "\n";
      edglog(debug) << "Error : " << oss.str() << endl;
      throw GaclException (__FILE__, __LINE__,  "GaclManager::addEntry" ,
                           WMS_GACL_ERROR,  oss.str() );
   };

   newCredential();
   setAllowPermission(type, rawvalue, permission);
};

/**
* Check whther the gacl file contains the specified entry
*/
bool GaclManager::hasEntry(const WMPgaclCredType& type, const string rawvalue)
{
   if (loadCredential(type, rawvalue) == WMPGACL_SUCCESS) {
      return true;
   } else {
      return false;
   }


};

/*
*  adds a set of credential entries to the gacl
*  @param vect vector of credential (credential type , rawvalue )
*/
void GaclManager::addEntries (const vector<pair<WMPgaclCredType, string> > &vect)
{
   for (unsigned int i = 0; i < vect.size() ; i++) {
      addEntry( (WMPgaclCredType)vect[i].first,
                (string)vect[i].second);
   }
};

/*
*  removes from the gacl file the entry with the specified credential
*/
int GaclManager::removeEntry (const WMPgaclCredType& type,
                              const string& rawvalue, string& errors)
{
   edglog_fn("GaclManager::removeEntry");
   std::ostringstream oss;
   GRSTgaclCred  *cred = NULL;
   GRSTgaclCred  *prev_cred = NULL;
   GRSTgaclEntry *entry = NULL;
   GRSTgaclEntry *prev_entry = NULL;
   GRSTgaclNamevalue *nv = NULL;
   bool found = false;
   // sets attributes related to the type of creddential
   setCredentialType (type,rawvalue);
   char* rawname =(char*) rawCred.first.c_str();
   if (gaclAcl !=NULL) {
      entry = gaclAcl-> firstentry ;
      // scanning of the entries
      while  (entry != NULL ) {
         cred = entry->firstcred ;
         // scanning of the credentials in the selected entry
         while ( cred != NULL ) {
               GRSTgaclCred *cred_tmp;

               cred_tmp = GRSTgaclCredNew((char*)credType.c_str());
               GRSTgaclCredAddValue(cred_tmp, rawname, (char*)rawvalue.c_str());

               if (GRSTgaclCredCmpAuri(cred, cred_tmp) == 0) {
                  found = true;
               }

               GRSTgaclCredFree(cred_tmp);
            if ( !found ) {
               prev_cred = cred ;
               cred = (GRSTgaclCred*) cred->next;
            } else {
               break;
            }
         } // while(cred)

         if ( !found ) {
            prev_entry = entry ;
            entry = (GRSTgaclEntry*) entry->next;
         } else {
            edglog(debug) << "entry found : removing .....\n";
            if (prev_entry) {
               prev_entry->next = (GRSTgaclEntry*) entry->next;
            } else {
               if ( entry->next ) {
                  gaclAcl-> firstentry = (GRSTgaclEntry*) entry->next;
               } else {
                  gaclAcl = NULL ;
               }
            }
            GRSTgaclEntryFree (entry);
            break;
         }
      } //while(entry)
   } //if
   else {
      oss << "Empty gacl file\n";
      edglog(debug) << "ACL is null" << "\n";
   }
   if (!found) {
      oss <<  "unable to remove the credential entry from the gacl" ;
      oss << " (" << gaclFile << ")\n";
      oss << "reason : the entry doesn't exist\ncredential type : " <<  getCredentialTypeString(type) << "\n";
      oss <<"input " << rawCred.first << ": " << rawvalue << "\n";
      errors += oss.str( ) ;
      return GaclManager::WMPGACL_ERROR ;
   } else {
      return GaclManager::WMPGACL_SUCCESS ;
   }
};

/*
 * Checks if permission is allowed
 * @param permission permission  (WMPGACL_xxxxx)
 *
 */
bool GaclManager::checkAllowPermission(const WMPgaclCredType& type,
                                        const string& rawvalue,
                                        const WMPgaclPerm& permission)
{

   edglog_fn("GaclManager::checkAllowPermission()");

   bool allow = false;
   bool deny = false;
   bool result = false;
   string errmsg = "";

   edglog(debug)<<"Raw Value = "<< rawvalue << endl;
   edglog(debug)<<"CredType/Permission = "<< type << "/"<< permission << endl ;

   // load Credential
   if (loadCredential(type, rawvalue) != WMPGACL_SUCCESS) {
      std::ostringstream oss;
      oss <<  "unable to check credential permission " ;
      oss << " (" << gaclFile << ")\n";
      oss << "(credential entry not found)\ncredential type: " << getCredentialTypeString(type)  << "\n";
      oss <<"input " << rawCred.first << ": " << rawvalue << "\n";
      edglog(debug) << oss.str()<< endl;
      result = false;
   }
   // permission
   allow = GaclManager::gaclAllowed & permission ;
   deny = GaclManager::gaclDenied & permission ;

   if ( allow && deny ) {
      errmsg = "gacl syntax error: operation both allowed and denied (" + gaclFile +")" ;
      edglog(debug) << errmsg << endl;
      throw GaclException (__FILE__, __LINE__,  "GaclManager::checkAllowPermission" ,
                           WMS_GACL_ERROR, errmsg );
   } else  {
      if ( !allow && !deny ) {
         result = false;
      } else {
         result = allow ;
      }
   }
   if (result) {
      edglog(debug) << "Success"<< endl;
   } else {
      edglog(debug) << "Failure"<< endl;
   }
   return result ;
};

/*
 * Checks if permission is denied
 */
bool GaclManager::checkDenyPermission(const WMPgaclCredType& type,
                                       const string& rawvalue, const WMPgaclPerm& permission)
{
   string errmsg = "";
   if (loadCredential(type, rawvalue) != WMPGACL_SUCCESS) {
      return false;
   };
   return GaclManager::gaclDenied & permission;

};

/*
 * allows permission to the user
 * @param permission permission (WMPGACL_xxxxx)
*/
void GaclManager::allowPermission(const WMPgaclCredType& type,
                                  const string& rawvalue,
                                  const WMPgaclPerm& permission,
                                  const bool& unset_perm)
{
   if (loadCredential(type, rawvalue) != WMPGACL_SUCCESS) {
      std::ostringstream oss;
      oss <<  "unable to set \"allow\" permission" ;
      oss << " (" << gaclFile << ")\n";
      oss << "reason : credential entry not found\ncredential type: " << getCredentialTypeString(type) << "\n";
      oss <<"input " << rawCred.first << ": " << rawvalue << "\n";
      throw GaclException (__FILE__, __LINE__,  "GaclManager::allowPermission" ,
                           WMS_GACL_ERROR,  oss.str() );
   };
   // reset permission if it is requested
   if (unset_perm) {
      gaclAllowed =GaclManager::WMPGACL_NOPERM ;
      gaclDenied =  gaclEntry->denied;
   }
   // undenies and allows "permission"
   GRSTgaclEntryAllowPerm (gaclEntry, permission) ;
   GRSTgaclEntryUndenyPerm (gaclEntry, permission) ;
   gaclAllowed = gaclEntry->allowed ;
   gaclDenied =  gaclEntry->denied;
};

/*
 * denies permission to the user
 * @param permission permission  (WMPGACL_xxxxx)
*/
void GaclManager::denyPermission(const WMPgaclCredType& type,
                                 const string& rawvalue,
                                 const WMPgaclPerm& permission,
                                 const bool& unset_perm)
{
   if (loadCredential(type, rawvalue) != WMPGACL_SUCCESS) {
      std::ostringstream oss;
      oss <<  "unable to set \"deny\" permission" ;
      oss << " (" << gaclFile << ")\n";
      oss << "reason: credential entry not found\ncredential type: " << getCredentialTypeString(type) << "\n";
      oss << "input " << rawCred.first << ": " <<rawvalue << "\n";
      throw GaclException (__FILE__, __LINE__,  "GaclManager::denyPermission" ,
                           WMS_GACL_ERROR,  oss.str() );
   };
   // reset permission if it is requested
   if (unset_perm) {
      gaclAllowed =GaclManager::WMPGACL_NOPERM ;
      gaclDenied =  gaclEntry->denied;
   }
   // denies and unallows "permission"
   GRSTgaclEntryDenyPerm (gaclEntry, permission) ;
   GRSTgaclEntryUnallowPerm (gaclEntry, permission) ;
   gaclAllowed = gaclEntry->allowed ;
   gaclDenied =  gaclEntry->denied;

};
/*
 * sets the allowed permission (it resets the old permission)
 *    @param permission permission  (WMPGACL_xxxxx)
*/
void GaclManager::setAllowPermission(const WMPgaclCredType& type,
                                     const string& rawvalue,
                                     const WMPgaclPerm& permission)
{

   // calls "allowPermission" with the unset_perm = true
   allowPermission(type, rawvalue, permission, true);
}

/*
 * sets the deniedpermission (it resets the old permission)
 *    @param permission permission  (WMPGACL_xxxxx)
*/
void GaclManager::setDenyPermission(const WMPgaclCredType& type,
                                    const string& rawvalue,
                                    const WMPgaclPerm& permission)
{

   // calls "allowPermission" with the unset_perm = true
   denyPermission(type, rawvalue, permission, true);
}

/*
 * saves the gacl to the file
 * @param file location where to save the file
*/
int GaclManager::saveGacl (const string& file)
{
   int result = -1;
   if ( gaclAcl ) {
      result = GRSTgaclAclSave (gaclAcl, (char*)file.c_str() );
   } else {
      newGacl ( );
      GRSTgaclAclSave (gaclAcl, (char*)file.c_str() );
   }
   if ( result == 0) {
      return WMPGACL_ERROR;
   } else {
      return WMPGACL_SUCCESS;
   }
};

/*
 * saves the gacl in a file
*/
int GaclManager::saveGacl ( )
{
   return saveGacl (gaclFile);
};
/*
* Gets the filepath
*/
std::string GaclManager::getFilepath ( )
{
   return gaclFile;
};

/*
* Checks for the credential type entry in the gacl file
*/
bool GaclManager::checkCredentialEntries(const std::string& type)
{

   GRSTgaclCred  *cred = NULL;
   GRSTgaclEntry *entry = NULL;
   bool found = false;

   // gacl file not null
   if (gaclAcl != NULL) {
      // looking for entries
      entry = gaclAcl-> firstentry ;
      // scanning of the entries
      while  (entry != NULL ) {
         cred = entry->firstcred ;
         // scanning of the credentials in the selected entry
         while ( cred != NULL ) {
#ifndef GRST_VERSION
            // checking each entry against the user credential type
            if ( strcmp( cred->type, (char*)type.c_str()) == 0 ) {
               found = true;
            }
#else
            {
               char *auri;

               if ((auri = GRSTgaclCredGetAuri(cred)) != NULL) {
                  if      ((strcmp((char*)type.c_str(), "person") == 0) &&
                           (strncmp(auri, "dn:", 3) == 0)) {
                     found = true;
                  } else if ((strcmp((char*)type.c_str(), "voms") == 0) &&
                             (strncmp(auri, "fqan:", 5) == 0)) {
                     found = true;
                  } else if ((strcmp((char*)type.c_str(), "dn-list") == 0) &&
                             (strncmp(auri, "url:", 4) == 0)) {
                     found = true;
                  } else if ((strcmp((char*)type.c_str(), "hostname") == 0) &&
                             (strncmp(auri, "dns:", 4) == 0)) {
                     found = true;
                  } else if (strncmp(auri, "any-user", 3) == 0) {
                     found = true;
                  }
               }
            }
#endif
            // the credential type did not match
            if ( !found ) {
               cred = (GRSTgaclCred*) cred->next;
            } else {
               break;
            }
         } // while(cred)

         // the credential type was not found in the entry
         if ( !found ) {
            entry = (GRSTgaclEntry*) entry->next;
         } else {
            break;
         }
      } //while(entry)
   } //if
   else {
      edglog(debug) << "ACL is null" << endl;
   }
   return found;
};

/*
* Returns the string describing the type of credential
*/
std::string GaclManager::getCredentialTypeString ( const WMPgaclCredType& type)
{
   string str = "";
   switch (type) {
   case WMPGACL_UNDEFCRED_TYPE: {
      str = GaclManager::WMPGACL_NONE_CRED ;
      break;
   }
   case WMPGACL_ANYUSER_TYPE: {
      str = GaclManager::WMPGACL_ANYUSER_CRED ;
      break;
   }
   case WMPGACL_PERSON_TYPE: {
      str = GaclManager::WMPGACL_PERSON_CRED ;
      break;
   }
   case WMPGACL_DNLIST_TYPE: {
      str = GaclManager::WMPGACL_DNLIST_CRED ;
      break;
   }
   case WMPGACL_VOMS_TYPE: {
      str = GaclManager::WMPGACL_VOMS_CRED ;
      break;
   }
   case WMPGACL_DNS_TYPE: {
      str = GaclManager::WMPGACL_DNS_CRED;
      break;
   }
   default: {
      str = "";
      break;
   }
   }
   return str;
};
/*
 * checks whether the gacl file exists
 * @param file the location of the gacl file
*/
bool GaclManager::gaclExists ( )
{
   edglog_fn("GaclManager::gaclExists");
   edglog(debug) << "checking file gacl existence" << endl;
   struct stat buffer;
   if (stat(gaclFile.c_str(), &buffer)) {
      return 0;
   }
   return 1;
}

/*
 * checks whether the gacl file exists
 * @param file the location of the gacl file
*/
bool GaclManager::gaclExists (const std::string& file)
{
   edglog_fn("GaclManager::gaclExists");
   edglog(debug) << "checking file gacl existence" << endl;
   struct stat buffer;
   if (stat(file.c_str(), &buffer)) {
      return 0;
   }
   return 1;
}
/*
*  adds a set of credential entries to the gacl
*  @param vect vector of credential (credential type , rawvalue )
*/
void GaclManager::removeEntries (const vector<pair<WMPgaclCredType, string> > &vect )
{
   string errors = "";
   for (unsigned int i = 0; i < vect.size() ; i++) {
      removeEntry( (WMPgaclCredType)vect[i].first,
                   (string)vect[i].second, errors);
   }
   if ( errors.size( )>0 ) {
      throw GaclException (__FILE__, __LINE__,
                           "GaclManager::removeEntries" ,
                           WMS_GACL_ERROR,  errors);
   }
};
/*
*  ============================
*  PRIVATE METHODS
*  ============================
*/
/*
 * loads the gacl from a file
 * @param file path location of the gacl file
*/
void GaclManager::loadFromFile( const string& file )
{
   edglog_fn("GaclManager::loadFromFile");
   edglog(debug) << "loading gacl from file : [" << file << "]" << endl;
   gaclAcl = GRSTgaclAclLoadFile ( (char*) file.c_str() );
   if (gaclAcl == NULL) {
      edglog(debug) << "gacl file not loaded: gaclAcl is null" << endl;
      ostringstream oss ;
      oss << "unable to load gacl from file : [" << file << "] (contact the server administrator)";
      throw GaclException (__FILE__, __LINE__,  "GaclManager::GaclManager" ,
                           WMS_GACL_ERROR,  oss.str() );
   }
};
/*
 * creates new gacl
 *
*/
void GaclManager::newGacl ()
{
   gaclCred = NULL ;
   gaclUser = NULL ;
   // gacl init ....
   GRSTgaclInit ( ) ;
   gaclAcl = GRSTgaclAclNew ();
   if ( GaclManager::gaclAcl == NULL ) {
      string errmsg = "Fatal error: unable to create a new gacl";
      throw GaclException (__FILE__, __LINE__,  "newGacl( )" ,
                           WMS_GACL_ERROR, errmsg);
   }
}
const vector<string> GaclManager::getItems(const WMPgaclCredType& type)
{
   edglog_fn("GaclManager::getItems");
   GRSTgaclCred  *cred = NULL;
   GRSTgaclEntry *entry = NULL;
   vector<string> items ;
   //operation not allow for any-user credential
   if (type == WMPGACL_ANYUSER_TYPE) {
      string errmsg = "operation not allows for any-user credential ";
      errmsg = "reason: no identifiers for this type of credential\n";
      throw GaclException (__FILE__, __LINE__,  "getItems( )" ,
                           WMS_GACL_ERROR, errmsg);
   }
   //sets credential type
   setCredentialType(type, "");
   // looks for the specified credential
   if (gaclAcl !=NULL) {
      entry = gaclAcl-> firstentry;
      // scanning of the entries
      while (entry) {
         cred = entry->firstcred;
         // scanning of the credentials in the selected entry
         while (cred) {
            // credType=WMPGACL_XXXXX_TAG (any-user, person, ......)
            char *auri, *decoded;

            if ((auri = GRSTgaclCredGetAuri(cred)) != NULL) {
               if      ((strcmp((char*)credType.c_str(), "person") == 0) &&
                        (strncmp(auri, "dn:", 3) == 0)) {
                  decoded = GRSThttpUrlDecode(&auri[3]);
                  items.push_back(decoded);
                  free(decoded);
               } else if ((strcmp((char*)credType.c_str(), "voms") == 0) &&
                          (strncmp(auri, "fqan:", 5) == 0)) {
                  decoded = GRSThttpUrlDecode(&auri[5]);
                  items.push_back(decoded);
                  free(decoded);
               } else if ((strcmp((char*)credType.c_str(), "dn-list") == 0) &&
                          (strncmp(auri, "url:", 4) == 0)) {
                  decoded = GRSThttpUrlDecode(&auri[4]);
                  items.push_back(decoded);
                  free(decoded);
               } else if ((strcmp((char*)credType.c_str(), "hostname") == 0) &&
                          (strncmp(auri, "dns:", 4) == 0)) {
                  decoded = GRSThttpUrlDecode(&auri[4]);
                  items.push_back(decoded);
                  free(decoded);
               }
            }
            cred = (GRSTgaclCred*) cred->next;
         } // while(cred)
         // next entry .
         entry = (GRSTgaclEntry*) entry->next;
      } //while(entry)
   }
   return items ;
}
/*
   sets the internal class attributes related to
   the credential
*/
void GaclManager::setCredentialType(const WMPgaclCredType& type, const string& rawvalue)
{
   switch (type) {
   case WMPGACL_ANYUSER_TYPE : {
      credType = GaclManager::WMPGACL_ANYUSER_CRED ;
      rawCred = make_pair(GaclManager::WMPGACL_ANYUSER_TAG, rawvalue);
      break;
   }
   case WMPGACL_PERSON_TYPE: {
      credType = GaclManager::WMPGACL_PERSON_CRED ;
      rawCred = make_pair(GaclManager::WMPGACL_PERSON_TAG, rawvalue);
      break;
   }
   case  WMPGACL_DNLIST_TYPE: {
      credType = GaclManager::WMPGACL_DNLIST_CRED ;
      rawCred = make_pair(GaclManager::WMPGACL_DNLIST_TAG, rawvalue);
      break;
   }
   case  WMPGACL_VOMS_TYPE: {
      credType = GaclManager::WMPGACL_VOMS_CRED ;
      rawCred = make_pair(GaclManager::WMPGACL_VOMS_TAG, rawvalue);
      break;
   }
   case  WMPGACL_DNS_TYPE: {
      credType = GaclManager::WMPGACL_DNS_CRED ;
      rawCred = make_pair(GaclManager::WMPGACL_DNS_TAG, rawvalue);
      break;
   }
   default : {
      string errmsg = "credential type not supported";
      throw GaclException(__FILE__, __LINE__,
                          "GaclManager::setCredentialType",
                          WMS_GACL_ERROR, errmsg);
      break;
   }
   }
};

int GaclManager::loadCredential(const WMPgaclCredType& type,
                                  const string& rawvalue)
{
   edglog_fn("GaclManager::loadCredential");
   // sets attributes related to the type of credential
   setCredentialType (type,rawvalue);
   return loadCredential();
};

int GaclManager::loadCredential()
{
   edglog_fn("GaclManager::loadCredential");
   GRSTgaclCred *cred = 0;
   GRSTgaclEntry *entry = 0;
   GRSTgaclNamevalue *nv = 0;
   bool found = false;
   char* rawname = const_cast<char *>(rawCred.first.c_str());
   char* rawvalue = const_cast<char *>(rawCred.second.c_str()); // the user's fqan

   if (gaclAcl) {
      entry = gaclAcl->firstentry;
      // scanning of the entries
      while (entry) {
         cred = entry->firstcred;
         // scanning of the credentials in the selected entry
         while (cred) {
            // delegation 2
            if (strcmp(
               (char*)credType.c_str(),
               GaclManager::WMPGACL_ANYUSER_CRED) == 0)
            {
               found = true;
            } else if (strcmp(
               (char*)credType.c_str(),
               GaclManager::WMPGACL_PERSON_CRED) == 0) {
               GRSTgaclCred *cred_tmp;

               cred_tmp = GRSTgaclCredNew((char*)credType.c_str());
               GRSTgaclCredAddValue(cred_tmp, rawname, rawvalue);

               if (GRSTgaclCredCmpAuri(cred, cred_tmp) == 0) {
                  found = true;
               }

               GRSTgaclCredFree(cred_tmp);
            } else if (strcmp(
               (char*)credType.c_str(),
               GaclManager::WMPGACL_VOMS_CRED) == 0)
            { // Comparing the two fqans, using wildards
               if (compareFQAN(string(cred->auri), string(rawvalue))) {
                  found = true;
               }
            }

            if (!found) {
               cred = (GRSTgaclCred *)cred->next;
            } else {
               break;
            }
         } // while(cred)

         if (!found) {
            entry = (GRSTgaclEntry *)entry->next;
         } else {
            break;
         }
      } // while(entry)
   } else {
      edglog(debug) << "ACL is null\n";
   }

   if (entry) {
      // main pointers
      gaclEntry = entry ;
      gaclCred = cred ;
      gaclUser  = GRSTgaclUserNew (cred);
      // allowed and denied operations currently set
      gaclAllowed = entry->allowed ;
      gaclDenied = entry->denied;
   }
   if (found) {
      return WMPGACL_SUCCESS;
   } else {
      return WMPGACL_ERROR;
   }
};

/*
 * creates the credential
*/
void GaclManager::newCredential()
{
   // <rawname><rawvalue></rawname>
   char* rawname = const_cast<char*>(rawCred.first.c_str());
   char* rawvalue = const_cast<char*>(rawCred.second.c_str());
   // new acl
   if (!gaclAcl) {
      GRSTgaclInit();
      gaclAcl = GRSTgaclAclNew();
   }
   if (!gaclAcl) {
      string const errmsg("Fatal error: unable to create new gacl");
      throw GaclException (__FILE__, __LINE__,  "newCredential (WMPgaclCredType,string, string, string)" ,
                           WMS_GACL_ERROR, errmsg);
   }
   // new entry
   gaclEntry = GRSTgaclEntryNew ( );
   if ( gaclEntry == NULL ) {
      string errmsg = "Fatal error; unable to create a new gacl entry";
      throw GaclException(__FILE__, __LINE__,  "newCredential (WMPgaclCredType, string, string, string)" ,
                           WMS_GACL_ERROR, errmsg);
   }
   gaclCred = GRSTgaclCredNew ((char *)credType.c_str());
   if (strcmp(
      (char*)credType.c_str(),
      GaclManager::WMPGACL_ANYUSER_CRED) != 0)
   {
      GRSTgaclCredAddValue(gaclCred,rawname, rawvalue);
   }
   if (!gaclCred) {
      string errmsg = "Fatal error: unable to create new credential";
      throw GaclException (__FILE__, __LINE__,  "newCredential (string, string, string)" ,
                           WMS_GACL_ERROR, errmsg );
   }
   // new user
   gaclUser = GRSTgaclUserNew(gaclCred);
   if (!gaclUser) {
      string const errmsg("Fatal error: unable to create new user credential");
      throw GaclException(__FILE__, __LINE__, "newCredential (string, string, string)" ,
                           WMS_GACL_ERROR, errmsg);
   }
   // adds the created credential to the entry
   GRSTgaclEntryAddCred(gaclEntry, gaclCred);
   // adds the entry to the gacl
   GRSTgaclAclAddEntry(gaclAcl, gaclEntry);
};

}}}}
