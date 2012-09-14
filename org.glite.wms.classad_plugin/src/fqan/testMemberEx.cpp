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

namespace glite {
namespace wms {
namespace classad_plugin {
namespace fqan {

namespace {
bool fqancomparator(std::string const& fqan, std::string const& acl)
{
  static boost::regex
    fqan_e(
      "^([[:alnum:]/._-]*)"
      "(/Role=([[:alnum:]._-]*))?"
      "(/Capability=([[:alnum:]._-]*))?"
    ),
    acl_e(
      "^([\\*[:alnum:]/._-]*)"
      "(/Role=([\\*[:alnum:]._-]*))?"
      "(/Capability=([\\*[:alnum:]._-]*))?"
    );


  boost::smatch fqan_p, acl_p;

  if (boost::regex_match(fqan, fqan_p, fqan_e) &&
      boost::regex_match(acl, acl_p, acl_e)) { // both acl and fqan are sintattically correct

     string acl_vo_group_subgroups_field(acl_p[1].first, acl_p[1].second);
     string fqan_vo_group_subbroups_field(fqan_p[1].first, fqan_p[1].second);
     // Replace '*' with '.*' in order to allow wildchard match in acl
     boost::algorithm::replace_all(acl_vo_group_subgroups_field, "*",".*");
     if (boost::regex_match(
       fqan_vo_group_subbroups_field,
       boost::regex(acl_vo_group_subgroups_field))) {

       string acl_role_field(acl_p[2].first, acl_p[2].second);
       string fqan_role_field(fqan_p[2].first, fqan_p[2].second);

       if( fqan_role_field.empty() && acl_role_field.empty() ) {
         return true;
       }

       if( !fqan_role_field.empty() &&
           !acl_role_field.empty() ) {

         string acl_role_value(acl_p[3].first, acl_p[3].second);
         string fqan_role_value(fqan_p[3].first, fqan_p[3].second);

         boost::algorithm::replace_all(acl_role_value, "*",".*");
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
           !arg0.IsErrorValue()) {
      
        result.SetBooleanValue( false );
        eval_successful=true;

        ExprList::const_iterator el_it( el->begin() ); 
        ExprList::const_iterator const  el_e( el->end() );
        for( ; el_it != el_e; ++el_it ) {
          Value cArg, b;
          std::string op1,op2;
          if ((*el_it)->Evaluate( state, cArg )) {
            // for FQAN comparator both operands 
            // should evaluate to string
            if (!strcasecmp(name, "fqanMember") &&
              arg0.IsStringValue(op1) && cArg.IsStringValue(op2) &&
              boost::algorithm::istarts_with(op2,"VOMS:")) { // only entries 

              if (boost::algorithm::istarts_with(op1,"VOMS:")) op1=op1.substr(5);
              if (fqancomparator(op1,op2.substr(5))) {
              
                result.SetBooleanValue( true );
                break;
              }
            }
          }
        }
      }
    }
  }
  return eval_successful;
}

} // fqan      namespace closure
} // classad_plugin namespace closure
} // wms            namespace closure
} // glite          namespace closure
