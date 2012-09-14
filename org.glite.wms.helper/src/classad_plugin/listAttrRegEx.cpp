/*
 * File: listAttrRegEx.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 */

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

// $Id$

#include <classad_distribution.h>
#include <fnCall.h>
#include <vector>
#include <string>
#include <functional>
#include <regex.h>
#include <cassert>
#include <algorithm>

using namespace std;
#ifdef WANT_NAMESPACES
using namespace classad;
#endif

namespace glite {
namespace wms {
namespace classad_plugin {
		
typedef std::list<ExprTree*> expression_trace_type;
typedef std::pair<expression_trace_type*, AttributeReference*> predicate_context_type;
typedef std::unary_function<predicate_context_type, bool> unary_predicate;

struct match_pattern : public unary_predicate
{ 
  match_pattern (regex_t* r) : re(r) {}
  bool operator()(const predicate_context_type& ctx) const {
	
    ExprTree* reference_expr = 0;
    std::string name;
    bool absolute;
    ctx.second->GetComponents(reference_expr, name, absolute);
    if( regexec( re, name.c_str(), (size_t)0, NULL, 0 ) == 0 ) return true;
    return false;
  }
  regex_t* re;
};

//
// TODO: this function should replace the InsertAttributeInVector one in classad_utils
//  
template<class Predicate>
void find_attribute_if(std::vector<std::string>* v, ExprTree* e, Predicate predicate, bool deep_find = false)
{
  expression_trace_type exprTrace, exprStack;

  if (!e) return;
  else exprStack.push_back(e);

  while (!exprStack.empty()) {

    exprTrace.push_front( (e = exprStack.front()) );
    exprStack.pop_front();
    
    switch( e->GetKind() ) {
    case ExprTree::LITERAL_NODE: break;
    case ExprTree::OP_NODE: 
      {
	ExprTree* e1 = 0, *e2 = 0, *e3 = 0;
	Operation::OpKind ok;
	dynamic_cast<Operation*>(e)->GetComponents(ok, e1, e2, e3);
	if ( e3 ) exprStack.push_front(e3);
	if ( e2 ) exprStack.push_front(e2);
	if ( e1 ) exprStack.push_front(e1);
      }
      break;
    case ExprTree::FN_CALL_NODE: 
      {
	std::vector<ExprTree*> args;
	std::string fn;
	dynamic_cast<FunctionCall*>(e)->GetComponents(fn, args);
	for( int i = args.size(); i; i--) exprStack.push_front( args[i-1] );
      }
      break;
    case ExprTree::EXPR_LIST_NODE: 
      {
	std::vector<ExprTree*> args;
	dynamic_cast<ExprList*>(e)->GetComponents(args);
	for( int i = args.size(); i; i--) exprStack.push_front( args[i-1] );
      }	
      break;
    case ExprTree::ATTRREF_NODE: 
      {
	AttributeReference* a = dynamic_cast<AttributeReference*>(e);
	ExprTree* reference_expr = 0;
	std::string name;
	bool absolute;
	bool continue_find = false;
	a->GetComponents(reference_expr, name, absolute);

	const ClassAd*  parent_scope   = 0;
	parent_scope=a->GetParentScope();

	if (!reference_expr) { // simple reference "attr" and ".attr"...
	  // ...look-up the parent scope to solve the reference...
	  reference_expr = parent_scope->Lookup(name);
	  if(reference_expr) continue_find = true;
	  else if(predicate(std::make_pair(&exprTrace,a)) &&
			  find(v->begin(), v->end(), name) == v->end()) {
		          
		  v->push_back(name);
	  }
	  
	}
 	else if (parent_scope && deep_find) {
	  
	  // ...reference_expr is not a simple reference "expr.attr" 
 	  // and the deep resolution is requested.
	  std::string    reference_name;
	  Value          reference_value;
	  const ClassAd* reference_ad;
	  ExprTree*      expr;
	  dynamic_cast<AttributeReference*>(reference_expr)->
	    GetComponents(expr, reference_name, absolute);
	  
	  if (parent_scope->EvaluateAttr(reference_name, reference_value) &&
	      reference_value.IsClassAdValue(reference_ad)) {
	    
	    reference_expr=reference_ad->Lookup(name);
	    continue_find = expr && !(expr->GetKind()==ExprTree::LITERAL_NODE);
	    if(!continue_find && predicate(std::make_pair(&exprTrace,a)) &&
			                              find(v->begin(), v->end(), name) == v->end()) {

		                      v->push_back(name);
		}			      
	  }
	}
	
 	// ...if the referenced expression has not been evaluated yet...
	if (reference_expr &&
	    find(exprTrace.begin(), exprTrace.end(), reference_expr) == exprTrace.end()) {
	  
	  if (continue_find) exprStack.push_front(reference_expr); // ...further search needed.
	  else if (predicate(std::make_pair(&exprTrace,a)) &&
		   find(v->begin(), v->end(), name) == v->end()) {
	    v->push_back(name);
	  } 
	}
      }
      break;
    default:
      assert( false );
    }
  }
}

  template<class Predicate>
void deep_find_attribute_if(std::vector<std::string>* v, ExprTree* e, Predicate predicate)
{
  find_attribute_if(v, e, predicate, true);
}

bool listAttrRegEx(const char         *name,
			  const ArgumentList &arguments,
			  EvalState          &state,
			  Value              &result)
{
  bool  eval_successful = false;
  result.SetErrorValue();
  // We check to make sure that we passed exactly two arguments...
  if (arguments.size() == 2) {
    
    Value       arg1;
    std::string pattern;
    // ...the first argument should evaluate to a string value...
    if (arguments[0] -> Evaluate(state, arg1) &&
	arg1.IsStringValue(pattern)) {
      // ...the second argument should be an attribute reference
      if (arguments[1] -> GetKind() == ExprTree::ATTRREF_NODE) {
	
	// Now we compile the pattern...
	regex_t re;
	if( !regcomp( &re, pattern.c_str(), REG_EXTENDED|REG_NOSUB ) ) {
	  std::vector<std::string> attrs;
	  deep_find_attribute_if(&attrs, arguments[1], match_pattern(&re));
	  // if there is no matching attribute then the undefined
	  // value should be returned as result. Otherwise we have 
	  // to create an exprlist containing the quoted names of
	  // the attributes matching the regex...
	  eval_successful = !attrs.empty();
	  
	  if (!eval_successful) result.SetUndefinedValue();
	  else {
	    std::vector<ExprTree*> attrsList;
	    for(std::vector<std::string>::const_iterator a=attrs.begin();
		a != attrs.end(); a++) {
	      Value value;
	      value.SetStringValue(*a);
	      attrsList.push_back( Literal::MakeLiteral(value) );
	    }
	    ExprList* e = ExprList::MakeExprList(attrsList);
	    e->SetParentScope(state.curAd);
	    result.SetListValue(e);
	  }
	  // dispose memory created by regcomp()
	  regfree( &re );
	}
      }
    }
  }
  return eval_successful;
}

} // namespace classad_plugin
} // namespace wms
} // namespace glite
