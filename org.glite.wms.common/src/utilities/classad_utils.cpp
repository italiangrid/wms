// File: classad_utils.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include "glite/wms/common/utilities/classad_utils.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

classad::ClassAd*
parse_classad(std::string const& s)
{
  classad::ClassAd* ad = 0;
  classad::ClassAdParser parser;
  ad = parser.ParseClassAd(s);

  if (ad == 0) {
    throw CannotParseClassAd(s);
  }

  return ad;
}

classad::ClassAd*
parse_classad(std::istream& is)
{
  classad::ClassAd* ad = 0;
  classad::ClassAdParser parser;
  ad = parser.ParseClassAd(is);

  if (ad == 0) {
    throw CannotParseClassAd();
  }

  return ad;
}

ValueProxy unparse_expression(classad::ExprTree const& tree) { classad::EvalState state; classad::Value v;  tree.Evaluate(state, v); return ValueProxy("", v); }


std::string
unparse_classad(classad::ClassAd const& ad)
{
  std::string result;

  classad::ClassAdUnParser unparser;
  unparser.Unparse(result, &ad);

  return result;
}

ValueProxy::ValueProxy(std::string const& e, classad::Value const& v)
  : m_expression(e), m_value(v)
{
}

ValueProxy::operator bool() const
{
  bool result;
  if (! m_value.IsBooleanValue(result)) {
    throw InvalidValue(m_expression, "bool");
  }

  return result;
}

ValueProxy::operator int() const
{
  int result;
  if (! m_value.IsIntegerValue(result)) {
    throw InvalidValue(m_expression, "int");
  }

  return result;
}

ValueProxy::operator double() const
{
  double result;
  if (! m_value.IsNumber(result)) {
    throw InvalidValue(m_expression, "double");
  }

  return result;
}

ValueProxy::operator std::string() const
{
  std::string result;
  if (! m_value.IsStringValue(result)) {
    throw InvalidValue(m_expression, "std::string");
  }

  return result;
}

ValueProxy::operator classad::ClassAd const*() const
{
  classad::ClassAd const* result;
  if (! m_value.IsClassAdValue(result)) {
    throw InvalidValue(m_expression, "classad::ClassAd const*");
  }

  return result;
}

ValueProxy::operator classad::ExprList const*() const
{
  classad::ExprList const* result;
  if (! m_value.IsListValue(result)) {
    throw InvalidValue(m_expression, "classad::ExprList const*");
  }

  return result;
}

ValueProxy
evaluate(classad::ExprTree const& et)
{
  classad::Value v;
  et.Evaluate(v);
  return ValueProxy("", v);
}

bool
evaluate(classad::Literal const& l, std::string& value)
{
  classad::Value v;
  l.GetValue(v);
  return v.IsStringValue(value);
}

ValueProxy
evaluate_attribute(classad::ClassAd const& ad,
                   std::string const& attribute)
{
  classad::Value v;
  ad.EvaluateAttr(attribute, v);
  return ValueProxy(attribute, v);
}

ValueProxy
evaluate_expression(classad::ClassAd const& ad,
                    std::string const& expression)
{
  classad::Value v;
  ad.EvaluateExpr(expression, v);
  return ValueProxy(expression, v);
}

bool
match(classad::ClassAd const& lhs, classad::ClassAd const& rhs,
      std::string const& match_type)
{
  bool result = false;

  classad::ClassAd* lhs_ad(new classad::ClassAd(lhs));
  classad::ClassAd* rhs_ad(new classad::ClassAd(rhs));
  assert(lhs_ad != 0 && rhs_ad != 0);

  classad::MatchClassAd match_ad(lhs_ad, rhs_ad);
  match_ad.EvaluateAttrBool(match_type, result);

  return result;
}

bool
left_matches_right(classad::ClassAd const& lhs, classad::ClassAd const& rhs)
{
  return match(lhs, rhs, "leftMatchesRight");
}

bool
right_matches_left(classad::ClassAd const& lhs, classad::ClassAd const& rhs)
{
  return match(lhs, rhs, "rightMatchesLeft");
}

bool
symmetric_match(classad::ClassAd const& lhs, classad::ClassAd const& rhs)
{
  return match(lhs, rhs, "symmetricMatch");
}

double
rank(classad::ClassAd const& lhs, classad::ClassAd const& rhs,
     std::string const& rank_type)
{
  classad::ClassAd* lhs_ad(new classad::ClassAd(lhs));
  classad::ClassAd* rhs_ad(new classad::ClassAd(rhs));
  assert(lhs_ad != 0 && rhs_ad != 0);

  classad::MatchClassAd match_ad(lhs_ad, rhs_ad);
  try {
    return evaluate_attribute(match_ad, rank_type);
  } catch (InvalidValue&) {
    throw UndefinedRank();
  }
}

double
left_rank(classad::ClassAd const& lhs, classad::ClassAd const& rhs)
{
  return rank(lhs, rhs, "leftRankValue");
}

double
right_rank(classad::ClassAd const& lhs, classad::ClassAd const& rhs)
{
  return rank(lhs, rhs, "rightRankValue");
}

// possible alternative version (faster but probably unsafe)
//  bool match(classad::ClassAd const& lhs,
//             classad::ClassAd const& rhs,
//             std::string const& type_of_match)
//  {
//    bool result = false;

//    // according to Alain Roy the following is safe because after
//    // RemoveLeft/RightAd() the original classads are restored to their original
//    // value
//    // it's probably not safe in a multithreaded environment, because another
//    // thread can access the classad in the time window when it's modified
//    classad::MatchClassAd match_ad(const_cast<classad::ClassAd*>(&lhs),
//                                    const_cast<classad::ClassAd*>(&rhs));
//    match_ad.EvaluateAttrBool(type_of_match, result);
//    match_ad.RemoveLeftAd();
//    match_ad.RemoveRightAd();

//    return result;
//  }

// more traditional interface (to be completed)

bool
evaluate_attribute(classad::ClassAd const& ad,
                   std::string const& attribute,
                   std::string& value)
{
  return ad.EvaluateAttrString(attribute, value);
}

bool
evaluate_attribute(classad::ClassAd const& ad,
                   std::string const& attribute,
                   int& value)
{
  return ad.EvaluateAttrInt(attribute, value);
}

bool
evaluate_expression(classad::ClassAd const& ad,
                    std::string const& expression,
                    std::string& value)
{
  bool result = false;

  classad::Value v;
  if (ad.EvaluateExpr(expression, v)) {
    result = v.IsStringValue(value);
  }

  return result;
}

bool
evaluate_expression(classad::ClassAd const& ad,
                    std::string const& expression,
                    classad::ClassAd const*& value)
{
  bool result = false;

  classad::Value v;
  if (ad.EvaluateExpr(expression, v)) {
    result = v.IsClassAdValue(value);
  }

  return result;
}

bool
evaluate(classad::ClassAd const& ad,
         std::string const& expression,
         classad::ExprList const*& value)
{
  bool result = false;

  classad::Value v;
  if (ad.EvaluateExpr(expression, v)) {
    result = v.IsListValue(value);
  }

  return result;
}

}}}} // glite::wms::common::utilities

