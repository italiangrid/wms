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
 *  filename  : FileStreamConnection.cpp
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2002 by INFN
 ***************************************************************************/

/**
 * @file FileStreamConnection.cpp
 * @brief ?
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */

// $Id$

/** Includes header definitions. */
#include "glite/wms/common/ldif2classad/FileStreamConnection.h"
#include "LDAPFilterParser.h"
#include "glite/wms/common/ldif2classad/LDAPQuery.h"

#include <classad_distribution.h>

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {

/**
 * Costructor
 */
fstream_search_result_t::fstream_search_result_t(result_container_type_t *result) 
{
  this -> fsresult = result;
}

fstream_search_result_t::~fstream_search_result_t() 
{
  if( fsresult ) {
  
	while( !fsresult -> empty() ) {
		
		delete fsresult -> front();
		fsresult -> pop_front();
	} 
  	delete fsresult;
  }
}

/**
 * Concrete Validation test
 */
bool fstream_search_result_t::good() const { return ( fsresult != NULL ); }

bool fstream_search_result_t::empty() const { return ( good() && fsresult -> empty() ); }

/**
 * Concrete Entry Factory Method.
 * @return the first ldap result entry. The memory object should be destroyed by explicitly call delete on it.
 */
generic_result_entry_t*fstream_search_result_t:: make_first_entry() const
{
  fstream_result_entry_t* e = NULL;
  
  if( good() ) e = new fstream_result_entry_t( fsresult -> begin(), fsresult -> end() );
  return (e);
}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
fstream_result_entry_t::fstream_result_entry_t( const fstream_search_result_t::const_iterator& first,
						const fstream_search_result_t::const_iterator& last )
{ 
  this -> fsentry  = first;
  this -> noentry  = last; 
}  
  
/**
 * Concrete next entry assignment.
 * @return whether the next entry has been succesfully addressed, or not.
 */
bool fstream_result_entry_t::next()
{
  if( this -> good() ) {
    
    return (++fsentry != noentry );
  }
  return false;
}

/**
 * Concrete validation test
 * @return whether the entry is valid or not.
 */
bool fstream_result_entry_t::good () const 
{
  return ( fsentry != noentry );
}

/**
 * Concrete distinguished name extractor.
 * @return a string representation of entry's distinguished name
 */
std::string fstream_result_entry_t::distinguished_name() const 
{
  std::string dn;
  if( good() ) dn = (*fsentry) -> EvaluateAttrString("dn",dn);
  return dn;
}

/**
 * Concrete LDIFObject extractor.
 * @return a poiter to a LDIFObject.
 */
LDIFObject* fstream_result_entry_t::value() 
{
  object = LDIFObject( (*fsentry) );
  return &object;
} 

/**
 * Constructor.
 * @param source_name the source file for stream connection.
 */
FileStreamConnection::FileStreamConnection(const string& source_name )
{
  this -> source_name = source_name;
  this -> source_data = NULL;
}

/**
 * Destructor.
 */
FileStreamConnection::~FileStreamConnection()
{ 
  close();
  
  delete_source_data();
}

void FileStreamConnection::delete_source_data()
{
  if(source_data != NULL) {
    
    while( !source_data -> empty() ) {
      delete source_data -> front();
      source_data -> pop_front();
    } 
    delete source_data;
    source_data = NULL;
  }
}

/**
 * ?
 * @return true on success, false otherwise.
 */
bool FileStreamConnection::open()
{ 
  close();
  delete_source_data();
  source_stream.open( source_name.c_str() );
  
  return source_stream.is_open() && load_source_data();
}

/**
 * ?
 * @return ?
 */
generic_search_result_t* FileStreamConnection::execute( LDAPQuery* query ) 
{
  fstream_search_result_t::result_container_type_t *query_results = NULL;
  
  string requirement_str;
  if( to_requirements( query -> filter(), requirement_str ) ) {
  
  classad::ExprTree *requirement_expr;
  classad::ClassAdParser parser;
    
  requirement_expr = parser.ParseExpression(requirement_str);
  
  query_results = new fstream_search_result_t::result_container_type_t;
  
  for( source_data_container_t::const_iterator it = source_data -> begin();
       it != source_data -> end(); it++ ) {
    
    classad::Value value_result;
    bool  bool_result;
    
    if( (*it) -> EvaluateExpr(requirement_expr,value_result) &&
	value_result.IsBooleanValue(bool_result) )  {
      
      if(bool_result) {
	
	classad::ClassAd ad;
	if( query -> topics().empty() ) ad = *(*it);
        else {
	  
            for(vector<string>::const_iterator a = query -> topics().begin(); 
		a != query -> topics().end(); a++) {
	    	classad::ExprTree* e = NULL;
		e = (*it) -> Lookup((*a).c_str());
		if( e ) {
			ad.Insert((*a), e -> Copy());
		} 
	    }
	}
	classad::ClassAd* aa = new classad::ClassAd(ad); 	
	query_results -> push_back( aa );
      }
    }
  }
  delete requirement_expr;
  }
  return new fstream_search_result_t(query_results);
}

/**
 * ?
 * @return true on success, false otherwise.
 */
bool FileStreamConnection::close()
{
  source_stream.close();
  return !source_stream.is_open();
}

/**
 * Checks whether the input file stream is good or not.
 * @return true if connection to the input file stream is established, false otherwise.
 */
bool FileStreamConnection::is_established() const 
{  
  return source_stream.good();
}

/**
 * ?
 * @return 
 */
bool FileStreamConnection::load_source_data()
{
  delete_source_data();
  if( !is_established() ) return false;
  
  source_stream.seekg(0, ios::beg);
  
  if( source_stream.eof() ) return false;
  
  source_data = new source_data_container_t;

  string strobject;
  
  while( seek_data_object() ) {
    
    string strobject;
    
    if( read_data_object(strobject) ) {
      
      classad::ClassAdParser parser;
      classad::ClassAd *ad = NULL;
     
      if( (ad = parser.ParseClassAd(strobject)) != NULL ) {

	source_data -> push_back( ad );
      }
    }
  }  
  return true;
}

/**
 * ?
 * @return 
 */
bool FileStreamConnection::read_data_object(string& strAd)
{   
  char c;
  
  do  {
    source_stream.get(c);
    strAd += c;
  }
  while( !source_stream.eof() && c != ']' );
    
  if( c != ']' ) return false;
  return true;

}

/**
 * ?
 * @return 
 */
bool FileStreamConnection::seek_data_object()
{
  char c = '\0';
  do {
    source_stream.get(c);
  } while( !source_stream.eof() && c != '[' );
  
  if( c=='[' ) {
    source_stream.putback(c);
    return true;
  }
  return false;
} 

bool FileStreamConnection::to_requirements(const string& s, string& r)
{
  LDAPFilterParser parser;
  return ( parser.parse(s, r, multi_attributes) );
}

} // namespace ldif2classad
} // namespace common
} // namespace wms
} // namespace glite

