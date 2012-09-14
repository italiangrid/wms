/*
 * File: lb_rank-classad-plugin.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
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
namespace lb_rank {
		
extern bool successFraction      (const char *, const ArgumentList&, EvalState&, Value&);

static ClassAdFunctionMapping functions[] = 
  {
    { "successFraction",      	(void*) successFraction,      	0  },
    { "", NULL, 0 }
};

} // namespace lb_rank
} // namespace classad_plugin
} // namespace wms
} // namespace glite

namespace plugin = glite::wms::classad_plugin::lb_rank;

extern "C" 
{ 
  ClassAdFunctionMapping *Init(void) 
  {
    return plugin::functions; 
  }
}
