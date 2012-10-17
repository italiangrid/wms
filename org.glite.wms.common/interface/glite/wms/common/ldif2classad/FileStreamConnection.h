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
 *  filename  : FileStreamConnection.h
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2002 by INFN
 ***************************************************************************/

// $Id: FileStreamConnection.h,v 1.1.36.1 2010/04/08 12:49:01 mcecchi Exp $

/**
 * @file FileStreamConnection.h
 * @brief ?
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */

#ifndef _FILESTREAMCONNECTION_
#define _FILESTREAMCONNECTION_ 

#include <list>
#include <fstream>

#include "LDAPConnection.h"
#include "LDIFObject.h"

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {

class fstream_result_entry_t;
/**
 * Concrete search result.
 * This class specialises the abstract interface (@see generic_search_result_t)for concrete FileStreamConnection search result.
 */
class fstream_search_result_t : public generic_search_result_t 
{
public:
  typedef std::list<classad::ClassAd*> result_container_type_t;
  typedef result_container_type_t::const_iterator const_iterator;
  
  /**
   * Costructor
   */
  fstream_search_result_t(result_container_type_t *result);
  
  ~fstream_search_result_t();
  
  /**
   * Concrete Validation test
   */
  bool good() const;
  bool empty() const;

  /**
   * Concrete Entry Factory Method.
   * @return the first ldap result entry. The memory object should be destroyed by explicitly call delete on it.
   */
  generic_result_entry_t* make_first_entry() const;
  
private:
  /* A pointer to the concrete LDAP result returned by ldap search */
  result_container_type_t  *fsresult;
};

/**
 * Concrete FileStreamConnection result entry.
 * This class specialises the abstract interface @see generic_result_entry
 * for concrete FileStreamConnection search result entries.
 */
class fstream_result_entry_t : public generic_result_entry_t
{
public:
  /**
   * Costructor
   * @param first a reference iterator to the first result entry
   * @param last a reference iterator to the last result entry
   */
  fstream_result_entry_t( const fstream_search_result_t::const_iterator& first,
			  const fstream_search_result_t::const_iterator& last );
  
  /**
   * Concrete next entry assignment.
   * @return whether the next entry has been succesfully addressed, or not.
   */
  bool next();
  /**
   * Concrete validation test
   * @return whether the entry is valid or not.
   */
  bool good () const;

  /**
   * Concrete distinguished name extractor.
   * @return a string representation of entry's distinguished name
   */
  std::string distinguished_name() const;
  
  /**
   * Concrete LDIFObject extractor.
   * @return a poiter to a LDIFObject.
   */
  LDIFObject* value();

private:
  /* ? */
  fstream_search_result_t::const_iterator fsentry;
  /* ? */
  fstream_search_result_t::const_iterator noentry;
  /* ? */
  LDIFObject object;
};

/**
 * A Concrete connection to a file as information source.
 * This class implements all virtual superclass methods in order
 * to provide a fake concrete connection to a file containing published datas.
 * @see LDAPConnection
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */
class FileStreamConnection : public LDAPConnection
{
 public:
  typedef classad::ClassAd source_data_type_t;
  typedef std::list<source_data_type_t*> source_data_container_t; 
  /**
   * Constructor.
   */
  FileStreamConnection(const std::string&);
  
  /**
   * Distructor.
   */
  virtual ~FileStreamConnection();
  
  /**
   * Opens the source data file and creates a memory representation of its data.
   * This function overrides virtual LDAP Connection one.
   * @return true on success, false otherwise.
   */
  bool open();
  /**
   * Closes the source data file.
   * This function overrides virtual LDAP Connection one.
   * @return true on success, false otherwise.
   */
  bool close();
  /**
   * Performs search operations using ldap_search_st() 
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
  void use_multi_attribute_list(std::vector<std::string>* multi_attributes) { this -> multi_attributes = multi_attributes; }
  // private:
  void delete_source_data();
  bool load_source_data();
  bool read_data_object(std::string& s);
  bool seek_data_object();
  
  
  bool to_requirements(const std::string& s, std::string& r);

private:
  std::string source_name;
  std::ifstream source_stream;
  source_data_container_t *source_data;
  std::vector<std::string>*  multi_attributes;
};

#endif

} // namespace ldif2classad 
} // namespace common
} // namespace wms
} // namespace glite 

/*
  Local Variables:
  mode: c++
  End:
*/
