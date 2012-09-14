// File: workload_manager.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sys/types.h>          // getpid(), getpwnam()
#include <unistd.h>             // getpid()
#include <pwd.h>                // getpwnam()
#include <boost/program_options.hpp>
#include "dispatcher.h"
#ifdef GLITE_WMS_USE_FILELIST
#include "filelist_reader.h"
typedef glite::wms::manager::server::FileListReader input_reader_type;
#else
#include "jobdir_reader.h"
typedef glite::wms::manager::server::JobDirReader input_reader_type;
#endif
#include "RequestHandler.h"
#include "signal_handling.h"
#include "TaskQueue.hpp"
#include "pipedefs.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/ism/ism.h"
#include "ism_utils.h"
#include "dynamic_library.h"

namespace po = boost::program_options;
namespace server = glite::wms::manager::server;
namespace configuration = glite::wms::common::configuration;
namespace task = glite::wms::common::task;
namespace logger = glite::wms::common::logger;
namespace utilities = glite::wms::common::utilities;

namespace {

std::fstream edglog_stream;

std::ostream* out_stream_p = &std::cout;
std::ostream* err_stream_p = &std::cerr;

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

boost::shared_ptr<server::InputReader>
create_input_reader(configuration::Configuration const& config)
{
  return boost::shared_ptr<server::InputReader>(
    new input_reader_type(config.wm()->input())
  );
//   if (config.wm()->dispatcher_type() == "filelist") {
//     return boost::shared_ptr<server::InputReader>(
//       new server::FileListReader(config.wm()->input())
//     );
//   } else {
//     return boost::shared_ptr<server::InputReader>(
//       new server::JobDirReader(config.wm()->input())
//     );
//   }
}

} // {anonymous}

int
main(int argc, char* argv[])
try {

  std::string opt_pid_file;
  std::string opt_conf_file;

  po::options_description desc("Usage");
  desc.add_options()
    ("help", "display this help and exit")
    (
      "daemon",
      po::value<std::string>(&opt_pid_file),
      "run in daemon mode and save the pid in this file"
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

#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING
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
      logger::threadsafe::edglog.activate_log_rotation(
        config.wm()->log_file_max_size(),
        config.wm()->log_rotation_base_file(),
        config.wm()->log_rotation_max_file_number()
      );
    } else {
      get_err_stream() << "cannot open the log file " << log_file << '\n';
      return EXIT_FAILURE;
    }
#else
    boost::details::pool::singleton_default<
      logger::wms_log
    >::instance().init(
                     logger::wms_log::SYSLOG, 
                     (logger::wms_log::level)config.wm()->log_level()
    );

#endif


    std::ofstream pid_file(opt_pid_file.c_str());
    if (!pid_file) {
      Error("the pid file " << opt_pid_file << " is not writable");
      return EXIT_FAILURE;
    }
    if (daemon(0, 0)) {
      Error("cannot become daemon (errno = "<< errno <<")");
      return EXIT_FAILURE;
    }
    pid_file << ::getpid();

  } else { // not daemon
#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING
    logger::threadsafe::edglog.open(
      get_err_stream(),
      static_cast<logger::level_t>(config.wm()->log_level())
    );
#else
    boost::details::pool::singleton_default<
      logger::wms_log
    >::instance().init(
                    logger::wms_log::STDERR, 
                    (logger::wms_log::level) config.wm()->log_level()
    );

#endif

  }

  Info("This is the gLite Workload Manager, running with pid " << ::getpid());

  if (!server::signal_handling()) {
    Error("cannot initialize signal handling");
    return EXIT_FAILURE;
  }

  std::string const broker_lib(config.wm()->broker_lib());
  if (broker_lib.empty()) {
    Error("empty BrokerLib attribute in configuration");
    return EXIT_FAILURE;
  }

  Info("loading broker dll " << broker_lib);
  server::DynamicLibrary broker_dll(
    broker_lib,
    server::DynamicLibrary::immediate_binding
    | server::DynamicLibrary::global_visibility
  );

  // force the creation of the TaskQueue before entering multithreading
  Info("creating the task queue");
  server::TaskQueue the_task_queue;

  glite::wms::manager::main::ISM_Manager ism_manager;

  boost::shared_ptr<server::InputReader> input_reader(
    create_input_reader(config)
  );

  server::Dispatcher dispatcher(*input_reader, the_task_queue);
  server::RequestHandler request_handler;

  // at scope exit force all threads to exit
  // this is needed if the dispatcher and request handler threads exit for a
  // reason not related to a signal
  utilities::scope_guard force_quit_at_scope_exit(
    server::set_received_quit_signal
  );

  server::pipe_type d2rh(config.wm()->pipe_depth());
  Info("spawning dispatcher...");
  task::Task d(dispatcher, d2rh);
  Info("spawning request handler(s)...");
  task::Task r(request_handler, d2rh, config.wm()->worker_threads());

  Info("running...");

} catch (boost::program_options::unknown_option& e) {
  Error(e.what());
  return EXIT_FAILURE;
} catch (server::CannotLoadDynamicLibrary& e) {
  Error("cannot load dynamic library " << e.filename()
        << ": " << e.error());
  return EXIT_FAILURE;
} catch (std::exception& e) {
  Error("std::exception " << e.what());
  return EXIT_FAILURE;
} catch (...) {
  Error("unknown exception");
  return EXIT_FAILURE;
}
