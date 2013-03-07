/*
 * File: classad-plugin-loader.cpp
 * Author: Monforte Salvatore 
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id: classad_plugin_loader.cpp,v 1.1.2.1 2012/09/11 10:19:40 mcecchi Exp $

#include <iostream>
#include <boost/thread/once.hpp>

#include "classad_plugin_loader.h"
#include <classad_distribution.h>
#include <fnCall.h>

using namespace classad;
using namespace std;

namespace glite {
namespace wms {
namespace classad_plugin {

extern bool doMatch(const char *, const ArgumentList&, EvalState&, Value&);
extern bool listAttrRegEx(const char *, const ArgumentList&, EvalState&, Value&);
extern bool retrieveCloseSEsInfo(const char *, const ArgumentList&, EvalState&, Value&);
extern bool testMemberEx(const char *, const ArgumentList&, EvalState&, Value&);
extern bool successFraction(const char *, const ArgumentList&, EvalState&, Value&);

static ClassAdFunctionMapping functions[] = 
{
  { "fqanMember", (void *)testMemberEx, 0 },
  { "allMatch",      (void *)doMatch,   0 },
  { "anyMatch",      (void *)doMatch,   0 },
  { "",            NULL,                 0 }
};

namespace {

boost::once_flag f_once = BOOST_ONCE_INIT;

void load()
{
  //classad::FunctionCall::RegisterFunctions(functions);
  std::string name("fqanMember");
  classad::FunctionCall::RegisterFunction(name, testMemberEx);
  name = "allMatch";
  classad::FunctionCall::RegisterFunction(name, doMatch);
  name = "anyMatch";
  classad::FunctionCall::RegisterFunction(name, doMatch);
  name = "whichMatch";
  classad::FunctionCall::RegisterFunction(name, doMatch);
  name = "listAttrRegEx";
  classad::FunctionCall::RegisterFunction(name, listAttrRegEx);
  name = "retrieveCloseSEsInfo";
  classad::FunctionCall::RegisterFunction(name, retrieveCloseSEsInfo);
  name = "successFraction";
  classad::FunctionCall::RegisterFunction(name, successFraction);
}

}

classad_plugin_loader::classad_plugin_loader()	
{
  boost::call_once(load, f_once);
}

} // namespace classad_plugin
} // namespace wms
} // namespace glite
