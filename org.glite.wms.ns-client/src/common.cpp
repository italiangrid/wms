/*
 * common.cpp
 * 
 * Copyright (C) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include "common.h"

/*
//   If you want to add the logger enable these.
   #include "glite/wms/common/logger/edglog.h"
   #include "glite/wms/common/logger/manipulators.h"
   
   namespace logger = glite::wms::common::logger;
*/

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace client {

/**
 * Redefines << operator for ClassAds objects.
 * This method puts the given ClassAdon the output stream.
 * @param o the output stream to write to.
 * @param ad the ClassAd.
 * @return the up-to-date output stream.
 */
std::ostream& operator <<(std::ostream& o, classad::ClassAd& ad)
{
  classad::PrettyPrint printAd;
  std::string strAd;
  printAd.Unparse( strAd, &ad );
  o << strAd << std::endl;
  return o;
}

/**
 * Redefines << operator for ClassAds objects.
 * This method puts the given ClassAdon the fstream.
 * @param f the fstream to write to.
 * @param ad the ClassAd.
 * @return the up-to-date fstream.
 */
std::fstream& operator <<(std::fstream& f, classad::ClassAd& ad)
{
  classad::PrettyPrint printAd;
  std::string strAd;
  printAd.Unparse( strAd, &ad );
  f << strAd << std::endl;
  return f;
}

/** 
 * Returns a quoted string obtained from a vector.
 * @param r the quoted string to return.
 * @param vet the Vector of tokens.
 */
void get_quoted_values(const std::string& r, std::vector<std::string>& vet)
{
  unsigned int begin = r.find("\"",0);
  unsigned int end;
  while (begin != std::string::npos && begin<r.length() )
    {
      end = r.find_first_of("\"",begin+1);
      vet.push_back(r.substr(begin+1,end-begin-1));
      begin = r.find("\"",end+1);
    }
}

/**
 * Replace a substring within another string with a new
 * substring.
 * @param where the domain string.
 * @param what the string to search.
 * @param with the string to put instead.
 */
void replace(std::string& where, const std::string& what, const std::string& with) {
  while(where.find(what)!= std::string::npos)
    where.replace(where.find(what),what.length(),with); 
}

/**
 * Converts an hostname into its dot IP representation.
 * @param hostname the hostanem to convert.
 * @param ip a reference of the string which will receive the IP representation.
 * @return whether the conversion has been successful, or not.
 */
bool hostname_to_ip(const std::string& hostname, std::string& ip)
{
    struct hostent *result = NULL;
 
    if( (result = gethostbyname(hostname.c_str())) == NULL ) return false;
 
    struct in_addr in;
    (void) memcpy(&in.s_addr, *result->h_addr_list, sizeof (in.s_addr));
    ip = inet_ntoa(in);
    return true;
}

/**
 * Resolves an hostname, supposed to be an alias, into its CNAME.
 * @param hostname the hostanem to resolve.
 * @param resolved_name to be filled with DNS record A host entry.
 * @return whether the conversion has been successful, or not.
 */
bool resolve_host(const std::string& hostname, std::string& resolved_name)
{
    struct hostent *result = NULL;
 
    if( (result = gethostbyname(hostname.c_str())) == NULL ) return false;
    
    resolved_name = result -> h_name;
    return true;
}



bool set_expression(classad::ClassAd* ad, const std::string& what, const std::string& with)
{
  classad::ClassAdParser parser;
  classad::ExprTree* tree;
  
  bool result = (tree = parser.ParseExpression(with)) != NULL;
  
  if(result) ad -> Insert(what,tree);
  
  return result;
}

/**
 * This funztion gets a list value from a ClassAd and
 * transforms it into a Vector of string. Returns true on a
 * successful conversion, false otherwise.
 * @param from a reference to source ClassAd.
 * @param what the attribute's name.
 * @param l the destination vector.
 * @return true on success, false otherwise.
 */
bool getVectorValue(classad::ClassAd* from, const std::string& what, std::vector<std::string>& l)
{ 
  std::string where;
 
  classad::Value list_value;
  const classad::ExprList *expr_list;
  if( from -> EvaluateAttr(what, list_value) == true &&
      list_value.IsListValue( expr_list ) == true ) {
   
    classad::ExprListIterator it( expr_list );
   
    while( it.CurrentExpr() ) {
 
      classad::Value v;
      std::string s;
      if( it.CurrentValue(v) && v.IsStringValue(s) ) l.push_back( s );
      else return false;
      it.NextExpr();
    }
  }
  else return false;
 
  return true;
}

/**
 * This function sets a list value from a Vector of string as an attribute
 * of the given ClassAd. Returns true on a successful conversion, false otherwise.
 * @param to a reference to destination ClassAd.
 * @param name the attribute's name.
 * @param l the source vector.
 * @return true on success, false otherwise.
 */
bool setVectorValue(classad::ClassAd* to, const std::string& name, const std::vector<std::string>& values)
{
  std::string list_value = "{";
  for( std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); ) {  
      list_value.append( std::string("\"") + (*it) + std::string("\"") );
      if( ++it != values.end() ) list_value.append(",");	
  }
  list_value.append("}");
#if DEBUG
  copy(values.begin(), values.end(), std::ostream_iterator<std::string>(cout, " "));
  std::cout << "In string: " << list_values << std:endl;
#endif  
  return set_expression(to, name, list_value);  
}

/**
 * This function gets a list value from a ClassAd and
 * transforms it into a list of string. Returns true on a
 * successful conversion, false otherwise.
 * @param from a reference to source ClassAd.
 * @param what the attribute's name.
 * @param l the destination list of strings.
 * @return true on success, false otherwise.
 */
bool getListValue(classad::ClassAd* from, const std::string& what, std::list<std::string>& l)
{ 
  std::vector<std::string> v;
  
  bool result = getVectorValue(from, what, v);
  if (result) {
    copy(v.begin(), v.end(), l.begin());
  }

  return result;
}

/**
 * This function puts a list of string as an attribute of the given ClassAd.
 * Returns true on a successful conversion, false otherwise.
 * @param to a reference to destination ClassAd.
 * @param name the attribute's name.
 * @param l the source vector.
 * @return true on success, false otherwise.
 */
bool setListValue(classad::ClassAd* to, const std::string& name, const std::list<std::string>& values)
{
  std::vector<std::string> v;
  copy(values.begin(), values.end(), v.begin()); 
  return setVectorValue(to, name, v);
}

} // namespace client
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite


