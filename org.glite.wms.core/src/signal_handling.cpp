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

// File: signal_handling.c
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id: signal_handling.cpp,v 1.1.2.1.2.1.2.2.4.1 2011/01/13 11:03:56 mcecchi Exp $

#include <boost/lexical_cast.hpp>
#include <csignal>
#include <cxxabi.h>
#include <execinfo.h>

#include "signal_handling.h"
#include "glite/wms/common/logger/logger_utils.h"

namespace {

bool f_received_quit_signal = false;
boost::mutex::mutex f_mx;
sigset_t f_mask;
std::string synch_sig_msg;

std::string
demangle(const char* symbol) {
  size_t size;
  int status;
  char temp[128];
  char* demangled;
  //first, try to demangle a c++ name
  if (1 == sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp)) {
    if (NULL != (demangled = abi::__cxa_demangle(temp, 0, &size, &status))) {
      std::string result(demangled);
      free(demangled);
      return result;
    }
  }
  //if that didn't work, try to get a regular c symbol
  if (1 == sscanf(symbol, "%127s", temp)) {
    return temp;
  }
 
  //if all else fails, just return the symbol
  return symbol;
}

void handle_synch_signal(int signum, siginfo_t* info, void*ptr) {
  synch_sig_msg += "Got a synchronous signal (" +
    boost::lexical_cast<std::string>(signum)+ "), stack trace:\n";

  int const MAX_FRAME_LEN = 20;
  void *bt[MAX_FRAME_LEN];
  char **strings;
  size_t sz;
  sz = backtrace(bt, MAX_FRAME_LEN);
  strings = backtrace_symbols(bt, sz);
  for (unsigned int i = 0; i < sz; ++i) {
    synch_sig_msg += demangle(strings[i])  + '\n';
  }
  Error(synch_sig_msg);
  free(strings);
  exit(-1);
}

void setup_synch_signal_handler() {
  struct sigaction action;
  memset(&action, 0, sizeof(action));
  action.sa_sigaction = handle_synch_signal;
  action.sa_flags = SA_SIGINFO;
  int signum;
  if(sigaction(signum = SIGSEGV, &action, NULL) < 0) {
    Error("cannot install signal handler for signal: " << signum);
  }
  if(sigaction(signum = SIGABRT, &action, NULL) < 0) {
    Error("cannot install signal handler for signal: " << signum);
  }
  if(sigaction(signum = SIGILL, &action, NULL) < 0) {
    Error("cannot install signal handler for signal: " << signum);
  }
  if(sigaction(signum = SIGFPE, &action, NULL) < 0) {
    Error("cannot install signal handler for signal: " << signum);
  }
}

}

namespace glite {
namespace wms {
namespace manager {
namespace server {

void SignalHandler::operator()() {

    while (!f_received_quit_signal) {
      int signal_number;
      sigwait(&f_mask, &signal_number);
      boost::mutex::scoped_lock l(f_mx);
      switch (signal_number) {
        case SIGINT:
        case SIGTERM:
        case SIGQUIT:
          Info("caught signal " << signal_number);
          f_received_quit_signal = true;
          m_events.stop();
          break;
        case SIGPIPE:
          Info("caught signal " << signal_number);
          break;
        default:
          Debug("caught signal " << signal_number);
          break;
      }
    }
  }

bool init_signal_handler()
{
  setup_synch_signal_handler();

  sigemptyset(&f_mask);
  sigaddset(&f_mask, SIGPIPE);
  sigaddset(&f_mask, SIGINT);
  sigaddset(&f_mask, SIGTERM);
  sigaddset(&f_mask, SIGQUIT);
  
  sigset_t* const oldmask = 0; // don't care about the oldmask
  int sigmask_error = pthread_sigmask(SIG_BLOCK, &f_mask, oldmask);
  if (sigmask_error) {
    Error("pthread_sigmask failure (" << sigmask_error << ")\n");
    return false;
  }

  return true;
}

void set_received_quit_signal()
{
  boost::mutex::scoped_lock l(f_mx);
  f_received_quit_signal = true;
}

bool received_quit_signal()
{
  boost::mutex::scoped_lock l(f_mx);
  return f_received_quit_signal;
}

}}}} // glite::wms::manager::server
