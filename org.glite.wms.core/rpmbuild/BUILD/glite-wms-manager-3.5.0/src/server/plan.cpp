// File: plan.cpp
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#include "plan.h"
#include <classad_distribution.h>
#include "glite/wms/helper/Request.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

classad::ClassAd* Plan(classad::ClassAd const& ad)
{
  glite::wms::helper::Request request(&ad);

  while (!request.is_resolved()) {
    request.resolve();
  }

  classad::ClassAd const* res_ad = resolved_ad(request);
  return res_ad ? new classad::ClassAd(*res_ad) : 0;
}

}}}} // glite::wms::manager::server
