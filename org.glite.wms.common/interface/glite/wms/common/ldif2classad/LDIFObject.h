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
 *  filename  : LDIFObject.h 
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001 by INFN
 ***************************************************************************/

// $Id:

/**
 * @file LDIFObject.cpp
 * This file contains LDIF Object implementation.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */

#ifndef _LDIFOBJECT_
#define _LDIFOBJECT_

#include <iostream>
#include <map>
#include <string>
#include "glite/wms/common/utilities/edgstrstream.h"  // fix 2.95 vs 3.2 pb
#include <vector>
#include <classad_distribution.h>

namespace glite {
namespace wms {
namespace common {
namespace utilities {
}
namespace ldif2classad {

#define LEFT  1
#define RIGHT 2

struct UndefinedRankEx
{
};

/** Defines an LDIFValue type. */
typedef std::vector<std::string> LDIFValue;
/** Defines an LDIFAttribute type. */
typedef std::map<std::string,LDIFValue> LDIFAttributes;

/**
 * An LDIF Object.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */
class LDIFObject
{
 public:

  /**
   * Empty Constructor.
   * It does nothing.
   */
  LDIFObject();
  LDIFObject(classad::ClassAd*);

  /**
   * Constructor.
   * Creates a new LDIF Object reusing attributes of an existing one.
   * @param o teh existing LDIF Object.
   */
  LDIFObject(const LDIFObject&);
  /** 
   * Redefines '=' operator between LDIF Objects.
   * It establishes that two LDIF object are the same if
   * their attributes are the same.
   * @param o the LDIF Object to compare.
   */  
  LDIFObject& operator= (const LDIFObject& o );
  
  /**
   * Adds an attribute.
   * It pushes back the val attribute in the att item in attributes std::map.
   * @param att the attribute item.
   * @param the value to insert.
   */
  void add(const std::string&, const std::string&);
  
  void merge(const LDIFObject& o);

  /**
   * Evaluates an attribute.
   * If the std::string name exists in attribtues std::map, then the front value is put in value string.
   * @param name the attribute std::map item.
   * @param value the destination std::string.
   * @return true for sucessful evaluation, false otherwise.
   */
  bool EvaluateAttribute(const std::string&, std::vector<std::string>&) const;

  /**
   * Evaluates an attribute set.
   * If the std::string name exists in attribtues std::map, then all values are put in value std::vector.
   * @param name the attribute std::map item.
   * @param value the destination std::vector.
   * @return true for sucessful evaluation, false otherwise.
   */  
  bool EvaluateAttribute(const std::string&, std::string&) const;	

  classad::ClassAd* asClassAd( void ) const { return ExportClassAd(); }
  classad::ClassAd* asClassAd( std::vector< std::string >::const_iterator b, std::vector<std::string>::const_iterator e) const { return ExportClassAd( b,e ); }
  classad::ClassAd asClassAdNonPtr(
    std::vector< std::string >::const_iterator b,
    std::vector<std::string>::const_iterator e
  ) const { return ExportClassAdNonPtr(b,e); }
 private:
  void ParseValue( const std::string&, utilities::edgstrstream& ) const;
  void ParseMultiValue( const LDIFValue&, utilities::edgstrstream& ) const; 
  std::string as_string(const classad::Value& v);
  LDIFAttributes from_ad(classad::ClassAd *ad);
  classad::ClassAd* ExportClassAd ( void ) const;
  classad::ClassAd* ExportClassAd ( std::vector< std::string >::const_iterator, std::vector<std::string>::const_iterator ) const;
  classad::ClassAd ExportClassAdNonPtr(
    std::vector< std::string >::const_iterator,
    std::vector<std::string>::const_iterator
  ) const;
/**
 * Writes the LDIFObject to a stream.
 * Serializes LDIF Object info into an output stream.
 * @param s the output stream.
 * @param o the LDIF Object.
 * @return the written ostream.
 */
  friend std::ostream& operator << (std::ostream&, const LDIFObject&); 

  /** Attributes std::vector. */
 mutable LDIFAttributes attributes;
};

extern std::ostream& operator<<        (std::ostream&, const LDIFObject&);
extern bool     MatchClassifiedAd (classad::ClassAd *where, classad::ClassAd *what);
extern double   RankClassifiedAd  (classad::ClassAd *where, classad::ClassAd *what, int);

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
