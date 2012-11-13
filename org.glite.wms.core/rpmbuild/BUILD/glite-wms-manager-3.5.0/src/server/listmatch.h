// File: listmatch.h
// Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
// Author: Cinzia Di Giusto <Cinzia.DiGiusto@cnaf.infn.it>
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_LISTMATCH_H
#define GLITE_WMS_MANAGER_SERVER_LISTMATCH_H

#include <string>

namespace classad {
class ClassAd;
}
namespace glite {
namespace wms {
namespace manager {
namespace server {
            
bool match(
  classad::ClassAd const& jdl, 
  std::string const& result_file,
  int number_of_results, 
  bool include_brokerinfo
);


}}}} // glite::wms::manager::server

#endif
