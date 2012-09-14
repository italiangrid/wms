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

// File: main.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Marco Cecchi <marco.cecchi@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id: main.cpp,v 1.1.2.10.2.4.2.2.2.1.2.2.2.4 2012/02/27 09:42:56 mcecchi Exp $

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <unistd.h> // getpid()
#include <pwd.h> //getpwnam() passwd()

#include <boost/program_options.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"
#include "glite/wms/common/logger/logstream_ts.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/utilities/input_reader.h"
#include "glite/wms/common/utilities/jobdir_reader.h"

#include "glite/wms/ism/ism.h"
#include "glite/wms/ism/purchaser/ism-purchaser.h"

#include "ism_utils.h"
#include "dynamic_library.h"
#include "signal_handling.h"
#include "events.h"
#include "wm_real.h"
#include "dispatcher_utils.h"
#include "replanner.h"
#include "recovery.h"
#include "lb_utils.h"

namespace po = boost::program_options;
namespace configuration = glite::wms::common::configuration;
namespace logger = glite::wms::common::logger;
namespace utilities = glite::wms::common::utilities;
namespace purchaser = glite::wms::ism::purchaser;
namespace ism = glite::wms::ism;

using namespace glite::wms::manager::server;

namespace {

std::fstream edglog_stream;

std::ostream* out_stream_p = &std::cout;
std::ostream* err_stream_p = &std::cerr;

class run_in_loop
{
  boost::function<void()> m_function;
  int m_period;
  Events& m_events;
  int priority;

public:
  run_in_loop(boost::function<void()> const& f, int s, Events& e, int p)
    : m_function(f), m_period(s), m_events(e), priority(p)
  { }
  void operator()()
  {
    m_function();
    m_events.schedule_at(
      *this,
      std::time(0) + m_period,
      priority
    );
  }
};

std::ostream& get_out_stream()
{
  return *out_stream_p;
}

void set_out_stream(std::ostream& os)
{
  out_stream_p = &os;
}

std::ostream& get_err_stream()
{
  return *err_stream_p;
}

void set_err_stream(std::ostream& os)
{
  err_stream_p = &os;
}

// change the uid and gid to those of user
// no-op if user corresponds to the current effective uid
// only root can set the uid
bool set_user(std::string const& user)
{
  uid_t euid = ::geteuid();

  if (euid == 0 && user.empty()) {
    return false;
  }

  ::passwd* pwd(::getpwnam(user.c_str()));
  if (pwd == 0) {
    return false;
  }
  ::uid_t uid = pwd->pw_uid;
  ::gid_t gid = pwd->pw_gid;

  return 
    euid == uid
    || (euid == 0 && ::setgid(gid) == 0 && ::setuid(uid) == 0);
}

bool
is_recovery_enabled()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );

  return config.wm()->enable_recovery();
}

bool have_ism_threads()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.wm()->ism_threads();
}

int ism_update_rate()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.wm()->ism_update_rate();
}

std::string config_input()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.wm()->input();
}

std::string get_ice_input()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.ice()->input();
}

std::string get_jc_input()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.jc()->input();
}

boost::shared_ptr<utilities::InputReader>
create_input_reader(
  std::string const& input
)
{
  boost::shared_ptr<utilities::InputReader> result;

  try {
    result.reset(new utilities::JobDirReader(input));
  } catch(utilities::JobDirError const& e) {
    Error(e.what());
  }

  return result;
}

} // {anonymous}

int
main(int argc, char* argv[])
try {

  std::string opt_conf_file;

  po::options_description desc("Usage");
  desc.add_options()
    ("help", "display this help and exit")
    (
      "daemon", "run in daemon mode"
    )
    (
      "conf",
      po::value<std::string>(&opt_conf_file)->default_value("glite_wms.conf"),
      "configuration file"
    )
    ;
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    get_out_stream() << desc << '\n';
    return EXIT_SUCCESS;
  }

  configuration::Configuration config(
    opt_conf_file,
    configuration::ModuleType::workload_manager
  );

  std::string dguser(config.common()->dguser());
  if (!set_user(dguser)) {
    get_err_stream() << "cannot set the user id to " << dguser << '\n';
    return EXIT_FAILURE;
  }

  if (vm.count("daemon")) {

    // init logger and adjust the err stream to use the same stream

    std::string log_file = config.wm()->log_file();
    if (!log_file.empty()) {
      if (!std::ifstream(log_file.c_str())) {
        std::ofstream(log_file.c_str());
      }
      edglog_stream.open(
        log_file.c_str(),
        std::ios::in | std::ios::out | std::ios::ate
      );
    }
    if (edglog_stream) {
      set_err_stream(edglog_stream);
      logger::threadsafe::edglog.open(
        edglog_stream,
        static_cast<logger::level_t>(config.wm()->log_level())
      );
    } else {
      get_err_stream() << "cannot open the log file " << log_file << '\n';
      return EXIT_FAILURE;
    }
    if (daemon(0, 0)) {
      get_err_stream() << "cannot become daemon (errno = "<< errno << ")\n";
      return EXIT_FAILURE;
    }
  } else { // not daemon

    logger::threadsafe::edglog.open(
      get_err_stream(),
      static_cast<logger::level_t>(config.wm()->log_level())
    );
  }

  Info("This is the gLite Workload Manager, running with pid " << ::getpid());

  std::string const broker_lib(config.wm()->broker_lib());
  if (broker_lib.empty()) {
    Error("empty BrokerLib attribute in configuration");
    return EXIT_FAILURE;
  }

  Info("loading broker dll " << broker_lib);
  DynamicLibrary broker_dll(
    broker_lib,
    DynamicLibrary::immediate_binding | DynamicLibrary::global_visibility
  );

  std::string const input(config_input());
  boost::shared_ptr<utilities::InputReader> input_reader(
    create_input_reader(input)
  );
  if (!input_reader) {
    Error(
      "cannot create input reader of type " << input_reader->name()
      << " from " << input_reader->source()
    );
    return EXIT_FAILURE;
  }
  Info(
    "created input reader of type " << input_reader->name()
    << " from " << input_reader->source()
  );

  boost::shared_ptr<utilities::JobDir> to_jc(
    new utilities::JobDir(get_jc_input())
  );
  boost::shared_ptr<utilities::JobDir> to_ice(
    new utilities::JobDir(get_ice_input())
  );

  // caching the jw template
  std::string template_file(
    config.wm()->job_wrapper_template_dir() + "/jobwrapper.template.sh"
  );
  std::ifstream ifs(template_file.c_str());
  if (!ifs) {
   get_err_stream() << "cannot open input file " << template_file;
   return EXIT_FAILURE;
  }
  std::ostringstream oss;
  oss << ifs.rdbuf();
  boost::shared_ptr<std::string> jw_template_ptr(
    new std::string(oss.str())
  );
  ifs.close();

  init_lb();
  Events event_processor;
  WMReal wm(to_jc, to_ice);

  if (is_recovery_enabled() && "jobdir" == input_reader->name()) {
    Info("starting recovery");
    recovery(
      input_reader,
      event_processor,
      wm,
      jw_template_ptr
    );
  } else {

    Info("skipping recovery");
  }

  glite::wms::manager::main::ISM_manager ism_manager; // first sync purchasing

  {
    if (!init_signal_handler()) {
      Error("cannot initialize signal handling");
      return EXIT_FAILURE;
    } 
    // here comes multithreading
    SignalHandler sh(event_processor);
    // right after first synchronous purchasing, before any worker thread
    boost::thread::thread sht(sh);

    Info("spawning " << config.wm()->worker_threads() << " worker threads...");
    boost::thread_group worker_threads;
    for(int i = 0; i < config.wm()->worker_threads(); ++i) {
      worker_threads.add_thread(
        new boost::thread(boost::bind(&Events::run, &event_processor))
      );
    }

    Info("scheduling dispatcher...");
    Dispatcher dispatcher(
      event_processor,
      input_reader,
      wm,
      jw_template_ptr
    );
    event_processor.schedule(dispatcher);

    Info("scheduling ISM purchaser(s)...");
    std::vector<
      boost::shared_ptr<purchaser::ism_purchaser>
    >::const_iterator it = ism_manager.purchasers().begin();
    std::vector<boost::shared_ptr<
      purchaser::ism_purchaser>
    >::const_iterator const end = ism_manager.purchasers().end();

    boost::thread_group ism_threads;
    for ( ; it != end; ++it) {
      if (!have_ism_threads()) {
        // unfortunately, this cannot always be done safely
        event_processor.schedule(
          run_in_loop(
            boost::bind(
              &purchaser::ism_purchaser::operator(),
              *it
            ),
            (*it)->sleep_interval(),
            event_processor,
            ism_priority
          )
        );
      } else {
        ism_threads.add_thread(
          new boost::thread(
            boost::bind(
              &purchaser::ism_purchaser::operator(),
              *it
            )
          )
        );
      }
    }

    Info("scheduling ISM updater...");
    ism::call_update_ism_entries update_ism;  
    if (!have_ism_threads()) {
      event_processor.schedule(
        run_in_loop(
          boost::bind(
            &ism::call_update_ism_entries::operator(),
            update_ism
          ),
          ism_update_rate(),
          event_processor,
          ism_priority
        )
      );
    } else {
      ism_threads.add_thread(
        new boost::thread(
          boost::bind(
            &ism::call_update_ism_entries::operator(),
            update_ism
          )
        )
      );
    }

    if (config.wm()->enable_replanner()) {
      Info("scheduling replanner...");
      boost::shared_ptr<ReplannerState> state;
      try {
        state.reset(
          new ReplannerState(
            wm,
            event_processor,
            jw_template_ptr
          )
        );
      } catch (CannotCreateLBContext const& e) {
        Error("Cannot create LB context for Replanner (" << e.what() << ')');
      }
      event_processor.schedule(
        Replanner(state),
        replanner_priority
      );
    }

    Info("WM startup completed...");
    worker_threads.join_all();
    ism_threads.join_all();
  } // dispatcher goes out of scope while worker threads have already
    // terminated, the ISM is still safely here

  exit(0); // nothing really important will be left at this point
} catch (CannotLoadDynamicLibrary const& e) {
  get_err_stream() << "cannot load dynamic library " << e.filename()
                   << ": " << e.error() << '\n';
  return EXIT_FAILURE;
} catch (utilities::JobDirError const& e) {
  Error(
    std::string(e.what()) + " while creating jobdir for either input or output"
  );
  return EXIT_FAILURE;
} catch (std::exception const& e) {
  get_err_stream() << "std::exception " << e.what() << "\n";
  return EXIT_FAILURE;
} catch (...) {
  get_err_stream() << "unknown exception\n";
  return EXIT_FAILURE;
}
