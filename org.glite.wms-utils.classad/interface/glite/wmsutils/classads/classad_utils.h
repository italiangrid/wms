// File: classad_utils.h
// Author: Salvatore Monforte <salvatore.monforte@ct.infn.it>
// Author: Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it> 
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMSUTILS_CLASSADS_CLASSAD_UTILS_H
#define GLITE_WMSUTILS_CLASSADS_CLASSAD_UTILS_H

#include <string>
#include <vector>
#include <stack>
#include <numeric>
#include <functional>
#include <algorithm>
#include <classad_distribution.h>
#include <iostream>
#include <cassert>

namespace glite {
namespace wmsutils {
namespace classads {

inline bool is_literal(classad::ExprTree const* t) {
  return t && t->GetKind() == classad::ExprTree::LITERAL_NODE;
}

inline bool is_attribute_reference(classad::ExprTree const* t)
{
  return t && t->GetKind() == classad::ExprTree::ATTRREF_NODE;
}

inline bool is_operation(classad::ExprTree const* t) {
  return t && t->GetKind() == classad::ExprTree::OP_NODE;
}

inline bool is_function_call(classad::ExprTree const* t) {
  return t && t->GetKind() == classad::ExprTree::FN_CALL_NODE;
}

inline bool is_classad(classad::ExprTree const* t)
{
  return t && t->GetKind() == classad::ExprTree::CLASSAD_NODE;
}

inline bool is_expression_list(classad::ExprTree const* t) {
  return t && t->GetKind() == classad::ExprTree::EXPR_LIST_NODE;
}

inline void setValue(classad::Value& value, const std::string& s) { value.SetStringValue(s); }
inline void setValue(classad::Value& value, double d) { value.SetRealValue(d); }
inline void setValue(classad::Value& value, bool b)   { value.SetBooleanValue(b); }
inline void setValue(classad::Value& value, int i)    { value.SetIntegerValue(i); }
  
inline bool getValue(const classad::Value& value, std::string& s) { return value.IsStringValue(s); }
inline bool getValue(const classad::Value& value, double& d)       { return value.IsRealValue(d); }
inline bool getValue(const classad::Value& value, bool& b)         { return value.IsBooleanValue(b); }
inline bool getValue(const classad::Value& value, int& i)          { return value.IsIntegerValue(i); }
  
template<class T> 
struct InsertExprInSet : public std::binary_function<std::vector<T>*, classad::ExprTree*, std::set<T>* > 
{
  std::set<T>* operator()(std::set<T>* v, classad::ExprTree* e)
  {
    if (is_literal(e)) {
      classad::Value value;
      static_cast<classad::Literal*>(e)->GetValue(value);
      T s;
      if (getValue(value, s)) {
        v->insert(s);
      }
    }
    return v;
  }
};

template<class T> 
struct InsertExprInVector : public std::binary_function<std::vector<T>*, classad::ExprTree*, std::vector<T>* > 
{
  std::vector<T>* operator()(std::vector<T>* v, classad::ExprTree* e)
  {
    if (is_literal(e)) {
      classad::Value value;
      static_cast<classad::Literal*>(e)->GetValue(value);
      T s;
      if (getValue(value, s)) {
        v->push_back(s);
      }
    }
    return v;
  }
};

template<class T> 
bool EvaluateAttrList(const classad::ClassAd& ad, const std::string& what, std::set<T>&l)
{
  bool                     res = false;
  std::string              where;
  classad::Value           list_value;
  const classad::ExprList *expr_list;
    
  if( ad.EvaluateAttr(what, list_value) == true &&
      list_value.IsListValue( expr_list ) == true ) {
    accumulate(expr_list -> begin(), expr_list -> end(), &l, InsertExprInSet<T>());
    res = true;
  }
    
  return res;
}

template<class T> 
bool EvaluateAttrList(const classad::ClassAd& ad, const std::string& what, std::vector<T>&l)
{
  bool                     res = false;
  std::string              where;
  classad::Value           list_value;
  const classad::ExprList *expr_list;
    
  if( ad.EvaluateAttr(what, list_value) == true &&
      list_value.IsListValue( expr_list ) == true ) {
    accumulate(expr_list -> begin(), expr_list -> end(), &l, InsertExprInVector<T>());
    res = true;
  }
    
  return res;
}

template<class T>
bool EvaluateAttrListOrSingle(const classad::ClassAd& ad, const std::string& what, std::vector<T>&l)
{
  bool res = false;
  if ( !(res = EvaluateAttrList(ad, what, l)) ) {
    
    classad::Value v;
    T tmpvalue;
    if ( res = (ad.EvaluateAttr(what, v) && getValue(v, tmpvalue) ) ) {
	    l.push_back(tmpvalue);
    }
  }
  return res;
}

template<class T>
T EvaluateExpr(classad::ClassAd& ad, const std::string& what)
{
  classad::Value v;
  T t;
  bool const b = ad.EvaluateExpr(what, v) && getValue(v,t);
  assert(b && "invalid return type for EvaluateExpr");
  return t;
}
  
template<class T> 
bool InsertAttrList(classad::ClassAd& ad, const std::string& what, const std::vector<T>&l)
{
  classad::ExprList* expr_list = asExprList(l);
    
  if( !expr_list ) return false;
    
  return ad.Insert(what,expr_list);
} 
  
inline std::string asString(classad::ClassAd& ad) 
{
  std::string s;
  classad::ClassAdUnParser unparser;
  classad::Value value;
	  
  value.SetClassAdValue(&ad);
  unparser.Unparse(s, value);

  return s;
} 
  
template<class T>
classad::ExprList* asExprList(const std::vector<T>& v)
{
  std::vector< classad::ExprTree* >          list;

  typename std::vector<T>::const_iterator it = v.begin();
  typename std::vector<T>::const_iterator const end = v.end();
  for ( ; it != end; ++it) {
    classad::Value value;
    setValue(value, (*it));
    list.push_back(classad::Literal::MakeLiteral(value));
  }
    
  classad::ExprList* result = classad::ExprList::MakeExprList(list);
    
  return result;
}
  
typedef std::list<classad::ExprTree*> expression_trace_type;
typedef std::pair<expression_trace_type*, classad::AttributeReference*> predicate_context_type;
typedef std::unary_function<predicate_context_type, bool> unary_predicate;

struct is_reference_to : public unary_predicate
{ 
  is_reference_to(const std::string& r) : ref(r) {}
  bool operator()(const predicate_context_type& ctx) const {
	
    classad::ExprTree* reference_expr = 0;
    std::string name;
    bool absolute;
    ctx.second->GetComponents(reference_expr, name, absolute);
    if (reference_expr && is_attribute_reference(reference_expr)) {
      classad::ExprTree* e;
      static_cast<classad::AttributeReference*>(reference_expr)->GetComponents(e, name, absolute);
      return name == ref;
    } 
    return false;
  }
  std::string ref;
};

struct is_absolute_reference : public unary_predicate
{ 
  bool operator()(const predicate_context_type& ctx) const {
    classad::ExprTree* reference_expr = 0;
    std::string name;
    bool absolute;
    ctx.second -> GetComponents(reference_expr, name, absolute);
    return absolute;
  }

};
    
struct always : unary_predicate
{
  bool operator()(const predicate_context_type& ctx) const { return true; }
};

struct is_function_call_to : public std::unary_function<classad::ExprTree*, bool>
{
   is_function_call_to(const std::string& fn) : fn_name( fn ) {}
   bool operator()(classad::ExprTree* e) const {
	   if (!is_function_call(e)) return false;
	   std::vector<classad::ExprTree*> args;
	   std::string fn;
           static_cast<classad::FunctionCall*>(e)->GetComponents(fn, args);
	   return fn == fn_name;
   }
   std::string fn_name;	   
};

struct is_operand_of : public unary_predicate
{
  is_operand_of(const std::string& fn) : fn_name( fn ) {}	
  bool operator()(const predicate_context_type& ctx) const {
  return find_if(ctx.first->begin(), ctx.first->end(), is_function_call_to(fn_name)) != ctx.first->end(); 
  }	  
  std::string fn_name; 
};

struct is_rightmost_operand_of : public unary_predicate
{
  is_rightmost_operand_of(const std::string& fn) : fn_name( fn ) {}
  bool operator()(const predicate_context_type& ctx) const {
	expression_trace_type::const_iterator it = find_if(ctx.first->begin(), ctx.first->end(), is_function_call_to(fn_name));
    	if( it == ctx.first->end() ) return false;
	
	std::vector<classad::ExprTree*> args;
	std::string fn;
	static_cast<classad::FunctionCall*>(*it)->GetComponents(fn, args);
  	return ctx.second == args.back();
  }
  std::string fn_name;
};

template<class Function>
std::vector<std::string>* insertAttributeInVector(std::vector<std::string>* v, classad::ExprTree* e, expression_trace_type* exprTrace, Function predicate)
{
  if( !e ) return v;
  
  exprTrace -> push_front(e);

  switch (e->GetKind()) {

  case classad::ExprTree::LITERAL_NODE:
    break;
      
  case classad::ExprTree::OP_NODE: {
    classad::ExprTree* e1 = 0, *e2 = 0, *e3 = 0;
    classad::Operation::OpKind ok;
    static_cast<classad::Operation*>(e)->GetComponents(ok, e1, e2, e3);
    if (e1) insertAttributeInVector(v, e1, exprTrace, predicate);
    if (e2) insertAttributeInVector(v, e2, exprTrace, predicate);
    if (e3) insertAttributeInVector(v, e3, exprTrace, predicate);
  }
    break;

  case classad::ExprTree::FN_CALL_NODE: {
    std::vector<classad::ExprTree*> args;
    std::string fn;
    static_cast<classad::FunctionCall*>(e)->GetComponents(fn, args);

    std::vector<classad::ExprTree*>::const_iterator it = args.begin();
    std::vector<classad::ExprTree*>::const_iterator const end = args.end();
    for ( ; it != end; ++it) {
      insertAttributeInVector(v, *it, exprTrace, predicate);
    }
  }
    break;

  case classad::ExprTree::EXPR_LIST_NODE: {
    std::vector<classad::ExprTree*> args;
    static_cast<classad::ExprList*>(e)->GetComponents(args);
    std::vector<classad::ExprTree*>::const_iterator it = args.begin();
    std::vector<classad::ExprTree*>::const_iterator const end = args.end();
    for ( ; it != end; ++it) {
      insertAttributeInVector(v, *it, exprTrace, predicate);
    }
  }
    break;

  case classad::ExprTree::ATTRREF_NODE: {
    
    classad::AttributeReference* a = static_cast<classad::AttributeReference*>(e);
    classad::ExprTree* reference_expr = 0;
    std::string name;
    bool absolute;
    a->GetComponents(reference_expr, name, absolute);
    if(!reference_expr)  {
      reference_expr = a->GetParentScope()->Lookup(name);
      if (reference_expr && reference_expr != e) {
        insertAttributeInVector(v, reference_expr, exprTrace, predicate);
      }
    } else {
      if (predicate(std::make_pair(exprTrace,a))
          && find(v->begin(), v->end(), name) == v->end()) {
        v->push_back(name);
      }
    }
  }
    break;

  default:
    assert(false && "Invalid ExprTree::GetKind()");
  }
  exprTrace->pop_front();
  return v;
}

template<class Function>
std::vector<std::string>* insertAttributeInVector(std::vector<std::string>* v, classad::ExprTree* e, Function predicate)
{
  expression_trace_type exprTrace;
  return insertAttributeInVector(v,e,&exprTrace, predicate);
}

inline std::vector<std::string>* insertAttributeInVector(std::vector<std::string>* v, classad::ExprTree* e)
{
  expression_trace_type exprTrace;
  return insertAttributeInVector(v,e,&exprTrace, always());
}

class ClassAdError: public std::exception
{
public:
  ~ClassAdError() throw() {};
  char const* what() const throw()
  {
    return "ClassAd utils - generic error";
  }
};

class CannotParseClassAd: public ClassAdError
{
  std::string m_what;
  std::string m_str;

public:
  CannotParseClassAd()
    : m_what("ClassAd utils - cannot parse classad")
  {
  }
  CannotParseClassAd(std::string const& ad_str)
    : m_what("ClassAd utils - cannot parse classad: " + ad_str),
      m_str(ad_str)
  {
  }
  ~CannotParseClassAd() throw() {}
  std::string str() const
  {
    return m_str;
  }
  char const* what() const throw()
  {
    return m_what.c_str();
  }
};
  
  // throws CannotParseClassAd
classad::ClassAd* parse_classad(std::string const& s);

  // throws CannotParseClassAd
classad::ClassAd* parse_classad(std::istream& is);

std::string unparse_classad(classad::ClassAd const& ad);

inline std::string unparse(classad::ExprTree const& et)
{
  std::string result;

  classad::ClassAdUnParser unparser;
  unparser.Unparse(result, &et);

  return result;
}

inline std::string unparse(classad::ExprTree const* et)
{
  return unparse(*et);
}

class InvalidValue: public ClassAdError
{
  std::string m_what;
public:
  InvalidValue(std::string const& expression, std::string const& type)
    : m_what("ClassAd error: attribute \"" + expression
                + "\" does not exist or has the wrong type (expecting \"" + type + "\")")
  {
  }
  ~InvalidValue() throw()
  {
  }
  char const* what() const throw()
  {
    return m_what.c_str();
  }
};
  
class ValueProxy
{
  std::string m_expression;
  classad::Value m_value;
public:
  ValueProxy(std::string const& e, classad::Value const& v);
  // all the following can throw InvalidValue
  operator bool() const;
  operator int() const;
  operator double() const;
  operator std::string() const;
  operator classad::ClassAd const*() const;
  operator classad::ExprList const*() const;
};

ValueProxy evaluate(classad::ExprTree const& et);
bool evaluate(classad::Literal const& et, std::string& value);

template<typename T>
bool evaluate(classad::ExprList const& el, std::vector<T>& value)
{
  bool result = false;

  classad::ExprList::const_iterator it = el.begin();
  classad::ExprList::const_iterator end = el.end();
  for ( ; it != end; ++it) {
    T t(evaluate(**it));
    value.push_back(t);
  }

  return result;
}

ValueProxy evaluate_attribute(classad::ClassAd const& ad,
                              std::string const& attribute);

ValueProxy evaluate_expression(classad::ClassAd const& ad,
                               std::string const& expression);

ValueProxy unparse_expression(classad::ExprTree const& tree);

bool
left_matches_right(classad::ClassAd& lhs, classad::ClassAd& rhs);
bool
left_matches_right(classad::ClassAd const& lhs, classad::ClassAd& rhs);
bool
left_matches_right(classad::ClassAd& lhs, classad::ClassAd const& rhs);
bool
left_matches_right(classad::ClassAd const& lhs, classad::ClassAd const& rhs);

bool
right_matches_left(classad::ClassAd& lhs, classad::ClassAd& rhs);
bool
right_matches_left(classad::ClassAd const& lhs, classad::ClassAd& rhs);
bool
right_matches_left(classad::ClassAd& lhs, classad::ClassAd const& rhs);
bool
right_matches_left(classad::ClassAd const& lhs, classad::ClassAd const& rhs);

bool
symmetric_match(classad::ClassAd& lhs, classad::ClassAd& rhs);
bool
symmetric_match(classad::ClassAd const& lhs, classad::ClassAd& rhs);
bool
symmetric_match(classad::ClassAd& lhs, classad::ClassAd const& rhs);
bool
symmetric_match(classad::ClassAd const& lhs, classad::ClassAd const& rhs);

class UndefinedRank: public ClassAdError
{
public:
  ~UndefinedRank() throw() {}
  char const* what() const throw()
  {
    return "ClassAd utils - undefined rank";
  }
};

double
left_rank(classad::ClassAd& lhs, classad::ClassAd& rhs);
double
left_rank(classad::ClassAd const& lhs, classad::ClassAd& rhs);
double
left_rank(classad::ClassAd& lhs, classad::ClassAd const& rhs);
double
left_rank(classad::ClassAd const& lhs, classad::ClassAd const& rhs);

double
right_rank(classad::ClassAd& lhs, classad::ClassAd& rhs);
double
right_rank(classad::ClassAd const& lhs, classad::ClassAd& rhs);
double
right_rank(classad::ClassAd& lhs, classad::ClassAd const& rhs);
double
right_rank(classad::ClassAd const& lhs, classad::ClassAd const& rhs);

// more traditional interface (to be completed)

bool evaluate_attribute(classad::ClassAd const& ad,
                        std::string const& attribute,
                        std::string& value);

bool evaluate_attribute(classad::ClassAd const& ad,
                        std::string const& attribute,
                        int& value);
  
bool evaluate_expression(classad::ClassAd const& ad,
                         std::string const& expression,
                         std::string& value);
  
bool evaluate_expression(classad::ClassAd const& ad,
                         std::string const& expression,
                         classad::ClassAd const*& value);

bool evaluate(classad::ClassAd const& ad,
              std::string const& expression,
              bool& value);
bool evaluate(classad::ClassAd const& ad,
              std::string const& expression,
              int& value);
bool evaluate(classad::ClassAd const& ad,
              std::string const& expression,
              double& value);
bool evaluate(classad::ClassAd const& ad,
              std::string const& expression,
              std::string& value);
bool evaluate(classad::ClassAd const& ad,
              std::string const& expression,
              classad::ClassAd const*& value);
bool evaluate(classad::ClassAd const& ad,
              std::string const& expression,
              classad::ExprList const*& value);

template<typename T>
bool evaluate(classad::ClassAd const& ad,
              std::string const& expression,
              std::vector<T>& v)
{
  classad::ExprList const* el;
  if (evaluate(ad, expression, el)) {
    return evaluate(el, v);
  }
}

}}}

#endif

// Local Variables:
// mode: c++
// End:
