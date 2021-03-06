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

// File: plan.cpp
// Author: Francesco Giacomini

// $Id: plan.cpp,v 1.1.2.1.4.2.2.2 2010/06/25 11:14:04 mcecchi Exp $

#include "plan.h"
#include <classad_distribution.h>
#include "glite/wms/helper/Request.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

classad::ClassAd* Plan(
  classad::ClassAd const& ad,
  boost::shared_ptr<std::string> jw_template
)
{
  glite::wms::helper::Request request(&ad, jw_template);

  while (!request.is_resolved()) {
    request.resolve();
  }

  classad::ClassAd const* res_ad = resolved_ad(request);
  return res_ad ? new classad::ClassAd(*res_ad) : 0;
}

}}}} // glite::wms::manager::server
