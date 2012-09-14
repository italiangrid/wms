// File: signal_handling.c
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html


#include "signal_handling.h"
#include <csignal>

static std::sig_atomic_t f_received_quit_signal;

extern "C" {
  void signal_handler(int)
  {
    f_received_quit_signal = 1;
  }
}

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace daemon {

void signal_handling_init()
{
  std::signal(SIGPIPE, SIG_IGN);
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);
  std::signal(SIGQUIT, signal_handler);
}

bool received_quit_signal()
{
  return f_received_quit_signal != 0;
}

}}}}} // glite::wms::manager::server
