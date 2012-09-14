// File: signal_handling.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_SIGNAL_HANDLING_H
#define GLITE_WMS_MANAGER_SERVER_SIGNAL_HANDLING_H

namespace glite {
namespace wms {
namespace manager {
namespace server {

bool signal_handling();
void set_received_quit_signal();
bool received_quit_signal();

}}}} // glite::wms::manager::server

#endif
