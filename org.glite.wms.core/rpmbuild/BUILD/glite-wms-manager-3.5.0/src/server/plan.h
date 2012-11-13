// File: plan.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_PLAN_H
#define GLITE_WMS_MANAGER_SERVER_PLAN_H

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {
namespace manager {
namespace server {

classad::ClassAd* Plan(classad::ClassAd const& ad);

}}}} // glite::wms::manager::server

#endif
