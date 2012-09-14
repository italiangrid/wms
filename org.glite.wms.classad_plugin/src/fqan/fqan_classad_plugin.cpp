/*
 * File: fqan-classad-plugin.cpp
 * Author: Monforte Salvatore
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#include <classad_distribution.h>
#include <fnCall.h>

using namespace std;
#ifdef WANT_NAMESPACES
using namespace classad;
#endif

namespace glite {
namespace wms {
namespace classad_plugin {
namespace fqan {
		
extern bool testMemberEx           (const char *, const ArgumentList&, EvalState&, Value&);

static ClassAdFunctionMapping functions[] = 
  {
    { "fqanMember",	        (void*)testMemberEx,	        0  },	
    { "", NULL, 0 }
};

} // namespace fqan
} // namespace classad_plugin
} // namespace wms
} // namespace glite

namespace plugin = glite::wms::classad_plugin::fqan;

extern "C" 
{ 
  ClassAdFunctionMapping *Init(void) 
  {
    return plugin::functions; 
  }
}
