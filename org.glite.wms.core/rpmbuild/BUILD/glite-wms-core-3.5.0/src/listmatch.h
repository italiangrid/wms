/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// File: listmatch.h
// Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
// Author: Cinzia Di Giusto <Cinzia.DiGiusto@cnaf.infn.it>

// $Id: listmatch.h,v 1.1.2.1.4.2 2010/04/07 14:02:46 mcecchi Exp $

#ifndef GLITE_WMS_MANAGER_SERVER_LISTMATCH_H
#define GLITE_WMS_MANAGER_SERVER_LISTMATCH_H

#include <string>
#include <boost/shared_ptr.hpp>

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
