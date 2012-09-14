// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

/*
 * File: testMemberEx.cpp
 * Author: Monforte Salvatore
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$ 

#include <classad_distribution.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/regex.hpp>
#include <fnCall.h>
#include <string>
#include <vector>
#include "glite/wmsutils/classads/classad_utils.h"

using namespace std;
#ifdef WANT_NAMESPACES
using namespace classad;
#endif

namespace utils = glite::wmsutils::classads;
namespace ba = boost::algorithm;

namespace glite {
namespace wms {
namespace classad_plugin {

namespace {
bool fqancomparator(std::string const& fqan, std::string const& acl)
{
  static boost::regex
    fqan_e(
      "^(?:/VO=(?:[[:alnum:]._-]*)/GROUP=)?"
      "([[:alnum:]/._-]*)"
      "(/Role=([[:alnum:]._-]*))?"
      "(/Capability=([[:alnum:]._-]*))?",
      boost::regex::perl|boost::regex::icase
    ),
    acl_e(
      "^(?:/VO=(?:[[:alnum:]._-]*)/GROUP=)?"
      "([\\*[:alnum:]/._-]*)"
      "(/Role=([\\*[:alnum:]._-]*))?"
      "(/Capability=([\\*[:alnum:]._-]*))?",
      boost::regex::perl|boost::regex::icase
    );

  boost::smatch fqan_p, acl_p;

  if (boost::regex_match(fqan, fqan_p, fqan_e) &&
      boost::regex_match(acl, acl_p, acl_e)) { // both acl and fqan are sintattically correct

     string acl_vo_group_subgroups_field(acl_p[1].first, acl_p[1].second);
     string fqan_vo_group_subbroups_field(fqan_p[1].first, fqan_p[1].second);
     // Replace '*' with '.*' in order to allow wildchard match in acl
     ba::replace_all(acl_vo_group_subgroups_field, "*",".*");
     if (boost::regex_match(
       fqan_vo_group_subbroups_field,
       boost::regex(acl_vo_group_subgroups_field))) {

       string acl_role_field(acl_p[2].first, acl_p[2].second);
       string fqan_role_field(fqan_p[2].first, fqan_p[2].second);

       if( ( fqan_role_field.empty() && acl_role_field.empty() ) ||
           ( fqan_role_field.empty() && 
             ba::equals(acl_role_field, "/Role=NULL", ba::is_iequal())
           ) || 
           ( acl_role_field.empty() &&
             ba::equals(fqan_role_field, "/Role=NULL", ba::is_iequal())
           )
       ) {
         return true;
       }

       if( !fqan_role_field.empty() &&
           !acl_role_field.empty() ) {

         string acl_role_value(acl_p[3].first, acl_p[3].second);
         string fqan_role_value(fqan_p[3].first, fqan_p[3].second);

         ba::replace_all(acl_role_value, "*",".*");
         if (boost::regex_match(
           fqan_role_value, boost::regex(acl_role_value))) {
           return true;
         }

       }
     }
  }
  return false;
}

}

bool 
testMemberEx(
  const char *name, 
  const ArgumentList &arguments, 
  EvalState &state, 
  Value &result)
{
  bool  eval_successful = false;
  result.SetErrorValue();
  
  // We check to make sure that we passed exactly two arguments...
  if (arguments.size() == 2) {
    Value    arg0, arg1;

    // Evaluate the arg list
    if ( arguments[0]->Evaluate(state,arg0) &&
         arguments[1]->Evaluate(state,arg1)) {

#ifdef FLEXIBLE_MEMBER
      if (arg0.IsListValue() && !arg1.IsListValue()) {
    
        Value swap;

        swap.CopyFrom(arg0);
        arg0.CopyFrom(arg1);
        arg1.CopyFrom(swap);
      }
#endif
      const ExprList *el;
      // arg1 must be a list; arg0 must be comparable
      // since we're using strict comparison, arg0 can't be 'error'
      if ( arg1.IsListValue(el) && 
           !arg0.IsListValue() && !arg0.IsClassAdValue() &&
           !arg0.IsErrorValue() ) {
      
        result.SetBooleanValue( false );
        eval_successful=true;

        ExprList::const_iterator el_it( el->begin() ); 
        ExprList::const_iterator const  el_e( el->end() );

        std::string op1;
        if (arg0.IsStringValue(op1) &&        
            boost::algorithm::istarts_with(op1,"DENY:") ||
            boost::algorithm::istarts_with(op1,"VOMS:") ) 
        {
          std::string op1_prefix = op1.substr(0,5);
  
          for( ; el_it != el_e; ++el_it ) {
            Value cArg, b;
            std::string op2;
            if ((*el_it)->Evaluate( state, cArg )) {
            
              if (!strcasecmp(name, "fqanMember") &&
                cArg.IsStringValue(op2) &&              
                boost::algorithm::istarts_with(op2,op1_prefix)) { // only entries with same prefix as op1

                if (fqancomparator(op1.substr(5),op2.substr(5))) {
              
                  result.SetBooleanValue( true );
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
  return eval_successful;
}

} // classad_plugin namespace closure
} // wms            namespace closure
} // glite          namespace closure
