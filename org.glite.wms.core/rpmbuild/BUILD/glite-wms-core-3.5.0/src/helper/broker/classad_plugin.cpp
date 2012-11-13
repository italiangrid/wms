/*
 * File: classad-plugin.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Author: Marco Cecchi <marco.cecchi@cnaf.infn.it>
 */

#include <classad_distribution.h>
#include <fnCall.h>

using namespace std;
#ifdef WANT_NAMESPACES
using namespace classad;
#endif

namespace glite {
namespace wms {
namespace classad_plugin {

extern bool doMatch(const char *, const ArgumentList&, EvalState&, Value&);
extern bool listAttrRegEx(const char *, const ArgumentList&, EvalState&, Value&);
extern bool retrieveCloseSEsInfo(const char *, const ArgumentList&, EvalState&, Value&);
extern bool testMemberEx(const char *, const ArgumentList&, EvalState&, Value&);
extern bool successFraction(const char *, const ArgumentList&, EvalState&, Value&);
extern float measured_response_time(const char *, const ArgumentList&, EvalState&, Value&);

static ClassAdFunctionMapping functions[] = {
  {"anyMatch", (void*) doMatch, 0},
  {"allMatch", (void*) doMatch, 0},
  {"whichMatch", (void*) doMatch, 0},
  {"listAttrRegEx", (void*) listAttrRegEx, 0},
  {"retrieveCloseSEsInfo", (void*) retrieveCloseSEsInfo,0},
  {"fqanMember", (void*)testMemberEx, 0},
  {"successFraction", (void*)successFraction, 0},
  {"measuredResponseTime", (void*)measured_response_time, 0},
  {"", NULL, 0}
};

}}}

extern "C" 
{ 
  ClassAdFunctionMapping *Init(void) 
  {
    return glite::wms::classad_plugin::functions; 
  }
}
