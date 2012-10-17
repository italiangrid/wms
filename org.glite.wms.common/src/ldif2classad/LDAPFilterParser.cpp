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
 *  filename  : LDAPFilterParser.cpp
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001, 2002 by INFN
 ***************************************************************************/

/**
 * @file LDAPFilterParser.cpp
 * @brief 
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */

// $Id: LDAPFilterParser.cpp,v 1.3.36.1 2010/04/08 12:49:02 mcecchi Exp $
// $Date: 2010/04/08 12:49:02 $

#include <iostream>
#include <algorithm>
#include <cctype>

#include "LDAPFilterParser.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {

// Simple filters can be specified as attributetype=attributevalue.
// More complex filters are specified using a prefix notation according 
// to the following BNF:
//               
// <filter> ::= '(' <filtercomp> ')'
// <filtercomp> ::= <and> | <or> | <not> | <simple>
// <and> ::= '&' <filterlist>
// <or> ::= '|' <filterlist>
// <not> ::= '!' <filter>
// <filterlist> ::= <filter> | <filter> <filterlist>
// <simple> ::= <attributetype> <filtertype> <attributevalue>
// <filtertype> ::= '=' | '~=' | '<=' | '>='

LDAPFilterTokenizer::LDAPFilterTokenizer(const string& s)
{
  ipos = 0;
  BORflag = true;
  ifilter = s; //purify(s);  
}

bool LDAPFilterTokenizer::get_token(token_t& token)
{
  if( eof() ) return false;
  else {
	
    switch( ifilter[ipos] ) {
    
    case '(':
      token = token_t( token_t::bracket_open,  string(1,ifilter[ipos++])); 
      break;
      
    case ')': 
      token = token_t( token_t::bracket_close, string(1,ifilter[ipos++])); 
      break;
	  
    case '&':
    case '|':
    case '!': 
      token = token_t( token_t::filtercomp, string(1,ifilter[ipos++])); 
      break;
      
    case '=': 
      token = token_t( token_t::filtertype, string(1,ifilter[ipos++])); 
      break;
      
    case '>':
    case '<':
    case '~': { 
      if( ifilter[ipos+1] != '=' ) return false;
      token = token_t( token_t::filtertype, string(ifilter.substr(ipos,2).c_str()));
      ipos += 2;
    }
    break;
    
    default: 
      
      size_t pos;
      if( BORflag ) { 
	
	pos = ifilter.find_first_of("=~><",ipos);
	  
	if( pos != string::npos ) {
	  
	  token = token_t(token_t::simple_string, string(ifilter.substr(ipos, pos - ipos).c_str()));
	}
	else {
	  
	  token = token_t (token_t::simple_string, string(ifilter.substr(ipos).c_str()));
	}
      }
      else {
	
	pos = ifilter.find(")",ipos);
	
	if( pos != string::npos ) {
	  token = token_t(token_t::extended_string, string(ifilter.substr(ipos, pos - ipos).c_str()));
	}
	else {
	  
	  token = token_t (token_t::extended_string, string(ifilter.substr(ipos).c_str()));
	}
      }
      ipos = pos;
    }
  }
  return true;
}

/**
 * Removes all non quoted spaces from the string.
 * @param s a reference to a string to be purified.
 * @return the purified string.
 */
string LDAPFilterTokenizer::purify(const string& s) const
{ 
  string p;
  bool quoted = false;
  for(size_t i = 0; i<s.length(); i++)
    {
      if( s[i]=='\"' ) quoted = !quoted;
      if( (s[i]==' ' && quoted) || 
	  s[i]!=' ' ) p+= s[i];
    }
  return p;
}

bool LDAPFilterParser::parse(const string& filter, string& pfilter, vector<string>* multi_attributes)
{ 
  bool result = true;

  token_t token;
  token_t::type_t last_type = token_t::none;
  string simple_filter;
  
  LDAPFilterTokenizer tokenizer( filter );
  
  while( result == true &&
	 !tokenizer.eof() && 
	 tokenizer.get_token(token) ) {
  
    switch( token.type ) {

    case token_t::bracket_open:
    case token_t::filtercomp:   
      filtercomps.push( token );
      break;
    case token_t::bracket_close:
      if( filtercomps.empty() ) result = false;
      else {
        result = parse_expression( multi_attributes ); 
        if( filtercomps.top().type == token_t::bracket_open) 
		filtercomps.pop();
	else result = false;
      }
  break;
    case token_t::extended_string:
    case token_t::simple_string: 
      if( last_type == token_t::none || 
          last_type == token_t::bracket_open ) {
	
	token.type = token_t::attributetype;
	parsedterms.push( string(token.value.c_str()) );
      }
      else 
	if( last_type == token_t::filtertype ) {
	  
	  if( !isdigit(token.value[0]) ) token.value = string("\"") + token.value + string("\"");
	  token.type = token_t::attributevalue;
	  parsedterms.push( string(token.value.c_str()) );
	}
	else
	  result = false;
      break;
    case token_t::filtertype: 
      if( last_type == token_t::attributetype ) {
	if( token.value == "=") token.value = "==";
        filtercomps.push(token);
      }
      else result = false;
      break;
      
    default:
      cerr << "Unanbled token in  LDAPFilterTokenizer::get_token" << endl; 
    }
    
    if( token.type == token_t::filtertype ) 
      tokenizer.break_on_reserved( false );
    else
      tokenizer.break_on_reserved( true );
    
    last_type = token.type;
  }
  
  while( result && parsedterms.size()>1 ) {

    if( (result = parse_expression()) )
      	if(!filtercomps.empty()) filtercomps.pop(); 
  };
    
  if( result = (parsedterms.size() == 1) ) {
	
      pfilter = string(parsedterms.top().c_str());
      parsedterms.pop();
  }
  result = parsedterms.empty();
  return result;
}

bool LDAPFilterParser::parse_expression( vector<string>* multi_attributes )
{
  bool result = true;
      if( filtercomps.top().type == token_t::bracket_open ) filtercomps.pop();
	else if( filtercomps.top().type == token_t::bracket_close ) filtercomps.pop();
	     else if( (filtercomps.top().type == token_t::filtercomp ||
		       filtercomps.top().type == token_t::filtertype) &&
		      parsedterms.size() >= 2 ) {

			 string r;
			 r = make_expression( multi_attributes );
			 parsedterms.push( string(r.c_str()) );
			 filtercomps.pop();
		     }
		     else result = false;
   return result;
}

string LDAPFilterParser::make_expression( vector<string>* multi_attributes )
{
  string result_str;
  string op1, op2;
  op1 = parsedterms.top();
  parsedterms.pop();
  op2 = parsedterms.top();
  parsedterms.pop();
  result_str = string("(");

  if( filtercomps.top().type == token_t::filtertype &&
      multi_attributes != NULL &&
      find( multi_attributes -> begin(),
            multi_attributes -> end(),
            op2) != multi_attributes -> end() ) {

              if( filtercomps.top().value == "==" ) {
	
	        result_str += string("member(") + op2 + string(",") + op1 + string(")");
              }
  }
  else {
  
    result_str += op2 + filtercomps.top().value;
    if( filtercomps.top().type != token_t::filtertype &&
        filtercomps.top().value != "!") result_str += filtercomps.top().value;
          result_str += op1;
  }
  result_str += string(")");
  return result_str;
}

} // namespace ldif2classad
} // namespace common
} // namespace wms
} // namespace glite
