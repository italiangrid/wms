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

// File: signal_handling.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id: signal_handling.h,v 1.1.2.1.2.1.2.2 2010/04/07 14:02:46 mcecchi Exp $

#ifndef GLITE_WMS_MANAGER_SERVER_SIGNAL_HANDLING_H
#define GLITE_WMS_MANAGER_SERVER_SIGNAL_HANDLING_H

#include "events.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

class SignalHandler {
  glite::wms::manager::server::Events& m_events;
public:
  SignalHandler(glite::wms::manager::server::Events& e)
    : m_events(e) { }
  void operator()();
};

bool init_signal_handler();
void set_received_quit_signal();
bool received_quit_signal();

}}}} // glite::wms::manager::server

#endif
