/*
 * File: classad-plugin-loader.cpp
 * Author: Monforte Salvatore 
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#define ENABLE_SHARED_LIBRARY_FUNCTIONS

#include "classad_plugin_loader.h"
#include <classad_distribution.h>
#include <fnCall.h>
#include <boost/thread/once.hpp>
#include <algorithm>

namespace glite {
namespace wms {
namespace classad_plugin {

namespace {

boost::once_flag f_once = BOOST_ONCE_INIT;

char const* plugins[] = { 
  "libglite_wms_classad_plugin.so"
};

void load()
{
  classad::ClassAdParser parser;
  std::for_each(
    plugins, plugins + (sizeof(plugins)/sizeof(char const*)),
    classad::FunctionCall::RegisterSharedLibraryFunctions
  );
}

}

classad_plugin_loader::classad_plugin_loader()	
{
  boost::call_once(load, f_once);
}

} // namespace classad_plugin
} // namespace wms
} // namespace glite
