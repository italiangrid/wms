/*
Copyright (c) Members of the EGEE Collaboration. 2004. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

    http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
See the License for the specific language governing permissions and 
limitations under the License.
*/

//
// File: signalhandler.cpp
// Author: Giuseppe Avellino <egee@datamat.it>
//

#include "signalhandler.h"

#include "fcgios.h"
#include <cxxabi.h>
#include <cstring>
#include <execinfo.h>

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "utilities/logging.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace server {

using namespace std;

volatile sig_atomic_t handled_signal_recv = 0;
void handler(int code);

namespace {

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

void handle_synch_signal(int signum, siginfo_t* info, void* ptr) {
  edglog_fn("handle_synch_signal");
  std::string synch_sig_msg = "Got a synchronous signal (" +
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
  edglog(fatal) << synch_sig_msg << std::endl;
  exit(-1);
}

void setup_synch_signal_handler() {
  struct sigaction action;
  ::memset(&action, 0, sizeof(action));
  action.sa_sigaction = handle_synch_signal;
  action.sa_flags = SA_SIGINFO; // sa_sigaction instead of sa_handler
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

typedef void handler_func(int);
void install_signal(int signo, handler_func* func)
{
  struct sigaction act, old_act;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_handler = func;
  sigaction(signo, &act, &old_act);
}

} // anonymous namespace

void
initsignalhandler()
{
  setup_synch_signal_handler();

  install_signal(SIGUSR1, handler);
  install_signal(SIGPIPE, SIG_IGN);
  install_signal(SIGTERM, handler);
  install_signal(SIGXFSZ, handler);
  install_signal(SIGHUP, handler);
  install_signal(SIGINT, handler);
  install_signal(SIGUSR1, handler);
  install_signal(SIGQUIT, handler);

  handled_signal_recv = 0;
}

void handler(int code) {
	handled_signal_recv = code;
}

} // namespace server
} // namespace wmproxy
} // namespace wms
} // namespace glite
