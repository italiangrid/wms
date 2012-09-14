/*
 * File: classad-plugin-loader.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#define ENABLE_SHARED_LIBRARY_FUNCTIONS

#include "classad_plugin_loader.h"

#include <boost/thread/mutex.hpp>
#include <classad_distribution.h>

#include <fnCall.h>

namespace glite {
namespace wms {
namespace classad_plugin {

namespace {
boost::mutex f_mtx;
}

bool init::operator()(std::string const& name)	
{
  static classad::ClassAdParser init_parser;
  boost::mutex::scoped_lock lk(f_mtx);
  bool result(
    classad::FunctionCall::RegisterSharedLibraryFunctions(
      std::string("libglite_wms_"+name+"_classad_plugin.so").c_str())
  );
  if(!result) std::cerr << name << "plugin initialization failure: " << classad::CondorErrMsg << std::endl;
  return result;
}

bool init::operator()(std::vector<std::string> const& names)
{
  std::vector<std::string>::const_iterator it = names.begin();
  std::vector<std::string>::const_iterator const e = names.end();
  bool result = true;
  for( ; it != e; ++it) result &= this->operator()(*it);
  return result;
}

} // namespace classad_plugin
} // namespace wms
} // namespace glite
