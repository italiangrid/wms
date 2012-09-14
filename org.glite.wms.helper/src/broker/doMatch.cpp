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
 * File: doMatch.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#include <classad_distribution.h>
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
		
bool 
doMatch(const char *name, const ArgumentList &arguments, EvalState &state, Value &result)
{
  bool  eval_successful = false;
  result.SetErrorValue();
  
  // We check to make sure that we passed exactly two arguments...
  if (arguments.size() == 2) {
    Value    arg1;
    const ExprList *adList;
    // ...the first argument should evaluate to a list...
    if (arguments[0] -> Evaluate(state, arg1) &&
	arg1.IsListValue(adList)) {
      
      // ...the second argument should be either an expression 
      // operand node or a function call node...
      if (arguments[1] -> GetKind() == ExprTree::OP_NODE ||
	  arguments[1] -> GetKind() == ExprTree::FN_CALL_NODE) {
	
	// Now we prepare the match context used while looking for 
	// any matching ad in the adList.In order to evaluate correctly 
	// any attribute reference involved in the second argument we 
	// should use the same classad containing the anyMatch statement 
	// while matching. Moreover we should make a copy of the requirements 
	// expression passed as second argument and set the parent scope by 
	// inserting it into the just copied classad. Finally, we set the left
	// context which will be constant for any further match.
	MatchClassAd match;
	ClassAd* al = static_cast<classad::ClassAd*>(arguments[1]->GetParentScope()->Copy());
	al->Insert("requirements", arguments[1]->Copy());
	match.ReplaceLeftAd(al);

	std::vector<ExprTree*> ads, matching;
	adList->GetComponents(ads);
       
	for(std::vector<ExprTree*>::const_iterator it = ads.begin(); 
	    it != ads.end(); it++) {
	  
	  // Each expression in the list should be a classad...
	  if (!utils::is_classad(*it)) {
	    
	    result.SetErrorValue();
	    eval_successful = false;
	    break;
	  }

	  ClassAd* ar = static_cast<ClassAd*>((*it)->Copy());
	  match.ReplaceRightAd(ar);
	  
	  // If requirements does not evaluate to a boolean there is
	  // something seriously wrong and exit...
	  if (!match.EvaluateAttrBool("rightMatchesLeft", eval_successful)) {
	    
	    result.SetErrorValue();
	    eval_successful = false;
	    break;
	  }
	  // ...otherwise we have to set the result and exit depending 
	  // on which function was called.
	  if ( (!strcasecmp(name, "anyMatch") &&  eval_successful) ||
	       (!strcasecmp(name, "allMatch") && !eval_successful)) {
	    
	    result.SetBooleanValue(eval_successful);
	    break;
	  }
	  
	  if (!strcasecmp(name, "whichMatch") && eval_successful) {
	    
	    matching.push_back(dynamic_cast<ClassAd*>(*it));
	  }
	}
	// if the whichMatch was called we have to set the result to 
	// the classad which matched...
	if (!strcasecmp(name, "whichMatch")) {
	  
	  eval_successful = !matching.empty();
	  
	  // if there is no matching ad then the evaluation was successful
	  // and we have to set the result to undefined...
	  // ...otherwise we have to set the result to the matching ad(s)
	  if (!eval_successful) result.SetUndefinedValue();
	  else result.SetListValue(ExprList::MakeExprList(matching));
	}
      }
    }
  }
  return eval_successful;
}

} // classad_plugin namespace closure
} // wms            namespace closure
} // glite          namespace closure
