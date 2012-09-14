/*
 * File: classad-plugin-loader.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$
#ifndef GLITE_WMS_CLASSAD_PLUGIN_LOADER_H
#define GLITE_WMS_CLASSAD_PLUGIN_LOADER_H

#include <vector>
#include <string>

namespace glite {
namespace wms {
namespace classad_plugin {

struct init
{
  bool operator()(std::string const&);
  bool operator()(std::vector<std::string> const&);
};

} // namespace classad_plugin
} // namespace wms
} // namespace glite
#endif
